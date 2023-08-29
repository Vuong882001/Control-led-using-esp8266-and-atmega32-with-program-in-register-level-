#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
#include "Arduino.h"
#include "coredecls.h"
#include <pgmspace.h>
#include "gdb_hooks.h"
#include "esp8266_peri.h"
#include "user_interface.h"
#include "uart_register.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;
ESP8266WebServer server(80);

#define SSID "WIFI_FREE"
#define PASSWORD "123456789"
#define UART0   0
uint8_t i = 21;
volatile int status;
String webPage =
{
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "   <meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
  "  <title>Điều khiển thiết bị</title>"
  "  <meta name='viewport' content='width=device-width, initial-scale=1'>"
  "  <style>"
  "    .b{width: 100px;height: 40px;font-size: 21px;color: #FFF;background-color:#4caf50;border-radius: 10px;}"
  "    .t{width: 100px;height: 40px;font-size: 21px;color: #FFF;background-color:#f44336;border-radius: 10px;}"
  "  </style>"
  "</head>"
  "<body>"
  "<div style='width: 330px;height: auto;margin: 0 auto;margin-top: 70px'>"
  "<h1 align='center'>Điều khiển thiết bị qua WIFI</h1>"
  "  <table align='center'>"
  "    <tr>"
  "    <td><a href='/bat1'><button class='b'>Bật 1</button></a><td>"
  "    <td><a href='/tat1'><button class='t'>Tắt 1</button></a><td>"
  "    <tr>"
  "    <tr>"
  "    <td><a href='/bat2'><button class='b'>Bật 2</button></a><td>"
  "    <td><a href='/tat2'><button class='t'>Tắt 2</button></a><td>"
  "    <tr>"
  "    <tr>"
  "    <td><a href='/bat3'><button class='b'>Bật 3</button></a><td>"
  "    <td><a href='/tat3'><button class='t'>Tắt 3</button></a><td>"
  "    <tr>"
  "  </table>"
  "</div>"
  "</body>"
  "</html>"
};
void uart_init() {
    // Cấu hình UART0 với tốc độ baud là 4800
    WRITE_PERI_REG(UART_CLKDIV(UART0), UART_CLK_FREQ / 4800);
    // Tắt chế độ chẳng lẻ và chẳng chẵn (parity) trong thanh ghi UART_CONF0
    CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_PARITY_EN | UART_PARITY | UART_STOP_BIT_NUM );
    // Cài đặt 8 bits truyền và 1 bit stop
    // Reset thanh ghi RX và TX
    SET_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST | UART_TXFIFO_RST); // set len 1 de xoa bo nho fifo
    CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST | UART_TXFIFO_RST);// tra ve 0 mac dinh
}
void uart_send_char(uint8_t c){
  uint8_t fifo_cnt = ((READ_PERI_REG(UART_STATUS(UART0)) >> UART_TXFIFO_CNT_S)& UART_TXFIFO_CNT);
    if (fifo_cnt < 126) {
        WRITE_PERI_REG(UART_FIFO(UART0), c);
    }
}

void TrangChu()
{
  server.send(200, "text/html", webPage);
}
void bat1()
{
  uart_send_char('a');
  server.send(200, "text/html", webPage);
}
void tat1()
{
  uart_send_char('b'); 
  server.send(200, "text/html", webPage);
}
void bat2()
{
  digitalWrite(2, LOW); 
  server.send(200, "text/html", webPage);
}
void tat2()
{
  digitalWrite(2, HIGH); 
  server.send(200, "text/html", webPage);
}
void bat3()
{
  uart_send_char('c');
  server.send(200, "text/html", webPage);
}
void tat3()
{
  uart_send_char('d');
  server.send(200, "text/html", webPage);
}

void user_station_config(){
  char ssid[32] = SSID;
  char password[64] = PASSWORD;
  struct station_config point;
  point.bssid_set=0; // need not check MAC address
  os_memcpy(&point.ssid,ssid,32);
  os_memcpy(&point.password,password,64);
  wifi_station_set_config(&point);
}

void setup() {
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  uart_init();
  delay(3000);
  Serial.begin(4800);
  wifi_set_opmode	(0x01); // set chế độ station
  wifi_station_disconnect(); // ngắt kết nối wifi trước đó
  user_station_config(); // set ssid và password
  wifi_station_connect(); // bắt đầu kết nối wifi
  delay(100);
  digitalWrite(2,HIGH);
   while (wifi_station_get_connect_status()!=STATION_GOT_IP) {
     delay(500);
      uart_send_char('.');
   }
      digitalWrite(2,LOW);
      uart_send_char('O');
      uart_send_char('K');
  
  server.on("/", TrangChu);
  server.on("/bat1", bat1);
  server.on("/tat1", tat1);
  server.on("/bat2", bat2);
  server.on("/tat2", tat2);
  server.on("/bat3", bat3);
  server.on("/tat3", tat3);
  server.begin();
}

void loop() {
  server.handleClient();
}
