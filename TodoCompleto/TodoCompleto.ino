/*
 Eugenio Martínez 13-10842
 Adrián González 14-10433
 Internet de las cosas
 
*/
//Ethernet
#include <SPI.h>  //Importamos librería comunicación SPI
#include <Ethernet.h>  //Importamos librería Ethernet
byte mac[] = { 0x98, 0x4F, 0xEE, 0x01, 0xB7, 0x2D };//Ponemos la dirección MAC 
IPAddress ip(192,168,0,100); //Asingamos la IP al Galileo
EthernetServer server(80); //Creamos un servidor Web con el puerto 80 que es el puerto HTTP por defecto


//Acelerometro
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;

//DHT
#include "DHT.h"
#define DHTIN 2 
#define DHTOUT 3
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTIN,DHTOUT, DHTTYPE);

//Potenciometro
int potPin = A0;    // input pin para el potentiometro
int potVal = 0;  // variable para guardar el valor del potenciometro

//LED
int led=6; //Pin del led
String estado="OFF"; //Estado del Led inicialmente "OFF"
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
 
  // Inicializamos la comunicación Ethernet y el servidor
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
 
  pinMode(led,OUTPUT);
}
 
void loop()
{

  //Potenciometro
  potVal = analogRead(potPin);
  Serial.print("El valor del potenciometro es: ");
  Serial.println(potVal);
  
  //Acelerometro
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);

  //DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");


  //Servidor Web
  EthernetClient client = server.available(); //Creamos un cliente Web
  //Cuando detecte un cliente a través de una petición HTTP
  if (client) {
    Serial.println("new client");
    boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
    String cadena=""; //Creamos una cadena de caracteres vacía
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();//Leemos la petición HTTP carácter por carácter
        Serial.write(c);//Visualizamos la petición HTTP por el Monitor Serial
        cadena.concat(c);//Unimos el String 'cadena' con la petición HTTP (c). De esta manera convertimos la petición HTTP a un String
 
         //Ya que hemos convertido la petición HTTP a una cadena de caracteres, ahora podremos buscar partes del texto.
         int posicion=cadena.indexOf("LED="); //Guardamos la posición de la instancia "LED=" a la variable 'posicion'
 
          if(cadena.substring(posicion)=="LED=ON")//Si a la posición 'posicion' hay "LED=ON"
          {
            digitalWrite(led,HIGH);
            estado="ON";
          }
          if(cadena.substring(posicion)=="LED=OFF")//Si a la posición 'posicion' hay "LED=OFF"
          {
            digitalWrite(led,LOW);
            estado="OFF";
          }
 
        //Cuando reciba una línea en blanco, quiere decir que la petición HTTP ha acabado y el servidor Web está listo para enviar una respuesta
        if (c == '\n' && currentLineIsBlank) {
 
            // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
 
            //Página web en formato HTML
            client.println("<html>");
            client.println("<head>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1 align='center'>Eugenio y Adrian :)</h1><h3 align='center'>LED controlado por Servidor Web con Arduino</h3>");
            //Creamos los botones. Para enviar parametres a través de HTML se utiliza el metodo URL encode. Los parámetros se envian a través del símbolo '?'
            client.println("<div style='text-align:center;'>");
            client.println("<button onClick=location.href='./?LED=ON\' style='margin:auto;background-color: #84B1FF;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("ON");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=OFF\' style='margin:auto;background-color: #84B1FF;color: snow;padding: 10px;border: 1px solid #3F7CFF;width:65px;'>");
            client.println("OFF");
            client.println("</button>");
            client.println("<br /><br />");
            client.println("<b>LED = ");
            client.print(estado);
            client.println("</b><br />");

//Potenciometro
            client.println("<b>Potenciometro = ");
            client.print(potVal);
            client.println("</b><br />");
//Acelerometro
            client.println("<b>Acelerometro: ");
            client.println("</b><br />");
            client.println("<b>x = ");
            client.print(ax);
            client.println("<b> y = ");
            client.print(ay);
            client.println("<b> z = ");
            client.print(az);
            client.println("</b><br />");
            client.println("<b> gx = ");
            client.print(gx);
            client.println("<b> gy = ");
            client.print(gy);
            client.println("<b> gz = ");
            client.print(gz);
            client.println("</b><br />");
//DHT
            client.println("<b>DHT: ");
            client.println("<b>Temperatura en *C: ");
            client.print(t);
            client.println("</b><br />");
            client.println("<b>Temperatura en *F: ");
            client.print(f);
            client.println("</b><br />");
            client.println("<b>Humedad en %: ");
            client.print(h);
            client.println("</b><br />");
            client.println("<b>Indice de calor *F: ");
            client.print(hi);
            client.println("</b></body>");
            client.println("</html>");
            break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    //Dar tiempo al navegador para recibir los datos
    delay(1);
    client.stop();// Cierra la conexión
  }
}
