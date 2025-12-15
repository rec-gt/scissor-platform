#include "WiFi.h"                   /* should be installed together with ESP32 Arduino install */
#include <ESPmDNS.h>               /* should be installed together with ESP32 Arduino install */

//https://github.com/espressif/esp-idf/blob/v2.0/examples/protocols/mdns/main/mdns_example_main.c

//https://github.com/espressif/esp-idf/blob/master/docs/api-reference/protocols/mdns.rst

// use: avahi-discover (linux) to check avahi services

mdns_server_t * mdns = NULL;

void setup() {

  const time_t endTime = millis() +  15 * 1000; /* 15 sec */

  const char * hostname = "broekie";

  Serial.begin(115200);
  WiFi.begin();
  while ( WiFi.status() != WL_CONNECTED && millis() < endTime )
  {
    Serial.print( "." );
    vTaskDelay( 500 / portTICK_PERIOD_MS );
  }

  const char * arduTxtData[4] = 
  {
      "board=esp32",
      "tcp_check=no",
      "ssh_upload=no",
      "auth_upload=no"
  };
  esp_err_t err = mdns_init(TCPIP_ADAPTER_IF_STA, &mdns);
  if (err) {
      ESP_LOGE(TAG, "Failed starting MDNS: %u", err);
  }
/*
  //first set name and start MDNS
  ESP_ERROR_CHECK( mdns_set_hostname(mdns, hostname) );
  ESP_ERROR_CHECK( mdns_set_instance(mdns, hostname) );
  
  //add a service - watch the correct order!
  ESP_ERROR_CHECK( mdns_service_add(mdns, "_arduino", "_tcp", 3232) );
  ESP_ERROR_CHECK( mdns_service_txt_set(mdns, "_arduino", "_tcp", 4, arduTxtData) );
  //add another service 
  ESP_ERROR_CHECK( mdns_service_add(mdns, "_http", "_tcp", 80) );
  ESP_ERROR_CHECK( mdns_service_txt_set(mdns, "_http", "_tcp", 4, arduTxtData) );
  ESP_ERROR_CHECK( mdns_service_instance_set(mdns, "_http", "_tcp", "Aquacontrol32 WebServer") );
  
  */
  
  // put your setup code here, to run once:
  Serial.printf( "Check if %s is already present...\n", hostname );
  resolve_mdns_host( "vissen" );
}

void loop() {
  // put your main code here, to run repeatedly:

}

void resolve_mdns_host(const char * hostname)
{
    printf("mDNS Host Lookup: %s.local\n", hostname);
    //run search for 1000 ms
    if (mdns_query(mdns, hostname, NULL, 1000)) {
        //results were found
        const mdns_result_t * results = mdns_result_get(mdns, 0);
        //itterate through all results
        size_t i = 1;
        while(results) {
            //print result information
            printf("  %u: IP:" IPSTR ", IPv6:" IPV6STR "\n", i++, IP2STR(&results->addr), IPV62STR(results->addrv6));
            //load next result. Will be NULL if this was the last one
            results = results->next;
        }
        //free the results from memory
        mdns_result_free(mdns);
    } else {
        //host was not found
        printf("  Host Not Found\n");
    }
}