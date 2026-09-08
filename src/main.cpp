// SPDX-License-Identifier: GPL-3.0-or-later
#include <Arduino.h>
#include <driver/spi_slave.h>
#include <NMEA2000_esp32.h>
#include <N2kMessages.h>
#include "config.h"
#include "duet.h"
#include <cstring>

// No WiFi, Bluetooth, web server, OTA or flash writes during capture.
// Static receive buffers; no allocation or application code in interrupt context.
tNMEA2000_esp32 n2k(static_cast<gpio_num_t>(config::can_tx),
                   static_cast<gpio_num_t>(config::can_rx));
duet::State state;
constexpr size_t slot_count=4, buffer_size=24;
struct Slot {
    spi_slave_transaction_t transaction{};
    alignas(4) uint8_t data[buffer_size]{};
};
Slot slots[slot_count];
bool can_enabled=false;
uint32_t last_send=0, last_status=0, tx_failures=0;
uint8_t sid=0;
const unsigned long transmit_pgns[] PROGMEM={128259UL,128267UL,0};

void queue_slot(Slot& slot) {
    memset(slot.data,0,sizeof(slot.data));
    slot.transaction={};
    slot.transaction.length=buffer_size*8;
    slot.transaction.rx_buffer=slot.data;
    slot.transaction.user=&slot;
    ESP_ERROR_CHECK(spi_slave_queue_trans(HSPI_HOST,&slot.transaction,0));
}

void setup_capture() {
    // Defined idle values if the NASA tap cable is disconnected.
    pinMode(config::data,INPUT_PULLDOWN);
    pinMode(config::clock,INPUT_PULLUP);
    pinMode(config::chip_select,INPUT_PULLUP);
    spi_bus_config_t bus{};
    bus.mosi_io_num=config::data;
    bus.miso_io_num=-1; // strictly passive; no MISO wire to NASA
    bus.sclk_io_num=config::clock;
    bus.quadwp_io_num=-1;
    bus.quadhd_io_num=-1;
    bus.max_transfer_sz=buffer_size;
    spi_slave_interface_config_t slave{};
    slave.spics_io_num=config::chip_select;
    slave.mode=3; // HT1621 captures on WR rising edge, idle high
    slave.queue_size=slot_count;
    ESP_ERROR_CHECK(spi_slave_initialize(HSPI_HOST,&bus,&slave,SPI_DMA_DISABLED));
    for (auto& slot:slots) queue_slot(slot);
}

void setup_can() {
    const uint64_t mac=ESP.getEfuseMac();
    char serial[20];
    snprintf(serial,sizeof(serial),"%012llX",static_cast<unsigned long long>(mac));
    const uint32_t unique=static_cast<uint32_t>((mac^(mac>>21))&0x1fffff);
    n2k.SetN2kCANMsgBufSize(16);
    n2k.SetN2kCANReceiveFrameBufSize(64);
    // Externally powered: NET-S is physically unconnected. LEN 0, uncertified.
    n2k.SetProductInformation(serial,1,"DIY Duet Bridge","0.1.0","prototype",0,2101,0);
    // Library's default experimental manufacturer code; not NASA/Navico identity.
    n2k.SetDeviceInformation(unique,60,135,2046,4);
    n2k.SetInstallationDescription1("Experimental NASA Duet LCD bridge; no raw sonar input");
    n2k.SetMode(tNMEA2000::N2km_NodeOnly,32);
    n2k.EnableForward(false);
    n2k.ExtendTransmitMessages(transmit_pgns);
    n2k.Open(); // Address claiming and ISO/product requests are handled by the library.
}

double wire_value(double v) { return std::isfinite(v) ? v : N2kDoubleNA; }

void setup() {
    Serial.begin(115200);
    pinMode(config::arm,INPUT_PULLUP);
    // Keep transceiver recessive while no CAN peripheral is enabled.
    digitalWrite(config::can_tx,HIGH);
    pinMode(config::can_tx,OUTPUT);
    delay(50);
    setup_capture();
    can_enabled=DUET_COMMISSIONED && digitalRead(config::arm)==LOW;
    if (can_enabled) setup_can();
    Serial.printf("DIY Duet prototype 0.1.0; CAN=%s; depth_reference=%s\n",
        can_enabled?"ENABLED":"OFF (diagnostics only)",
        DUET_ZERO_OFFSET_VERIFIED?"zero verified":"UNVERIFIED: depth NA");
}

void loop() {
    const uint32_t now=millis();
    if (can_enabled) n2k.ParseMessages();
    spi_slave_transaction_t* finished=nullptr;
    // Bound each pass to avoid starving CAN service on noisy inputs.
    for (size_t i=0;i<slot_count;++i) {
        esp_err_t err=spi_slave_get_trans_result(HSPI_HOST,&finished,0);
        if (err==ESP_ERR_TIMEOUT) break;
        ESP_ERROR_CHECK(err);
        auto* slot=static_cast<Slot*>(finished->user);
        state.ingest(slot->data,sizeof(slot->data),finished->trans_len,now);
        queue_slot(*slot);
    }
    auto current=state.current(now,config::stale_ms);
    // Removing the jumper suppresses measurements immediately (CAN management remains).
    if (digitalRead(config::arm)!=LOW) current=duet::Reading{};
    if (can_enabled && uint32_t(now-last_send)>=config::send_ms) {
        last_send=now;
        tN2kMsg msg;
        const double depth=DUET_ZERO_OFFSET_VERIFIED ? wire_value(current.depth_m) : N2kDoubleNA;
        SetN2kWaterDepth(msg,sid,depth,
            depth==N2kDoubleNA ? N2kDoubleNA : config::offset_m,N2kDoubleNA);
        if (!n2k.SendMsg(msg)) ++tx_failures;
        SetN2kBoatSpeed(msg,sid,wire_value(current.speed_mps),N2kDoubleNA,N2kSWRT_Paddle_wheel);
        if (!n2k.SendMsg(msg)) ++tx_failures;
        sid=(sid+1)%253; // 253..255 reserved
    }
    if (uint32_t(now-last_status)>=1000) {
        last_status=now;
        // Diagnostics remain available even with ARM open.
        const auto observed=state.current(now,config::stale_ms);
        Serial.printf("frames=%lu rejected=%lu depth_m=%.3f stw_mps=%.3f screen=%d tx_fail=%lu\n",
            static_cast<unsigned long>(state.frames),static_cast<unsigned long>(state.rejected),
            observed.depth_m,observed.speed_mps,observed.normal_screen,
            static_cast<unsigned long>(tx_failures));
    }
    delay(1);
}
