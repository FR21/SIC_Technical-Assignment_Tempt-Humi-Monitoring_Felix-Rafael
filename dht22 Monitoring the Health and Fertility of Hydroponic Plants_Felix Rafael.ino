#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "FELDAVKWAN"; //ganti wifi ID
const char* password = "fdkay168"; //ganti wifi Password

#define tb_server "thingsboard.cloud"
const int tb_port = 1883;
#define tb_token "cvb5lagakl78o23yuazd" //ganti token Thingsboard

WiFiClient espClient;
PubSubClient client(espClient);
#define DHT_PIN 15 //ganti pin sensor DHT
#define DHT_TYPE DHT22 //ganti jenis sensor DHT

DHT dht(DHT_PIN, DHT_TYPE);


void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Koneksi WiFi...");
  }
  Serial.println("Terhubung ke WiFi");


  dht.begin();

  client.setServer(tb_server, tb_port);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();


  // Baca suhu dan kelembaban dari sensor DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    // Kirim data ke ThingsBoard
    String telemetryData = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    char attributes[telemetryData.length() + 1];
    telemetryData.toCharArray(attributes, telemetryData.length() + 1);

    client.publish("v1/devices/me/telemetry", attributes);
    Serial.println("Data terkirim: " + telemetryData);
  }

  delay(5000); // Kirim data setiap 5 detik
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Tambahkan kode callback jika diperlukan
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke ThingsBoard...");
    if (client.connect("ESP32_Client", tb_token, "")) {
      Serial.println("Berhasil");
    } else {
      Serial.print("Gagal, status=");
      Serial.print(client.state());
      Serial.println(" Coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}