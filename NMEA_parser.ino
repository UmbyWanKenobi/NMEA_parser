
/*
   NMEA parser
   (c) 2017 Lafayette Software Development for Il Poliedrico
   written by Umberto Genovese
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
  //
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  //
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
  //
  Hardware richiesto:
  1 Motherboard Arduino Mega 2560 R3 ATmega2560-16AU
  1 GY-GPS6MV1/2 ricevitore GPS
*/
String NMEA;
int pos = 0;
int index = 0;
int field = 0;
#define GPS Serial1
String tmp;
float val;
typedef struct times {
  uint8_t year;
  uint8_t month;
  uint8_t date;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};
typedef struct latitude {
  uint8_t DD;
  uint8_t MM;
  uint8_t SS;
  float ddSS;
  String dir;
  float mantissa;
  float dec;
};
typedef struct longitude {
  uint8_t HH;
  uint8_t MM;
  uint8_t SS;
  float ddSS;
  String dir;
  float mantissa;
  float dec;
};

times TIME;
latitude LATITUDE;
longitude LONGITUDE;

void setup() {
  Serial.begin(115200);
  GPS.begin(9600);
}

void fix() {
  if (GPS.available()) {
    char c = GPS.read();
    if (c != '\n') {
      NMEA += c;
    }
    else {
      extract_string_data ();
    }
  }
}

void extract_string_data () {
  pos = 0;
  index = 0;
  field = 0;
  if (NMEA.substring(0, 6) == "$GPRMC")
  {
    while (pos != -1)
    {
      pos = NMEA.indexOf(",", index);
      switch (field)
      {
        case 1:          //   ORA    195403.00
          tmp = (NMEA.substring(index, pos));
          TIME.hour = tmp.substring(0, 2).toInt();
          TIME.minute = tmp.substring(2, 4).toInt();
          TIME.second = tmp.substring(4, 6).toInt();

        //      case 2:          //
        case 3:          //   LATITUDINE 1234.01234
          tmp = (NMEA.substring(index, pos));
          val =  tmp.substring(4, 10).toFloat();
          LATITUDE.DD = tmp.substring(0, 2).toInt();
          LATITUDE.MM  = tmp.substring(2, 4).toInt();
          LATITUDE.SS = (val * 60) ;
          LATITUDE.ddSS = ((val * 60) - LATITUDE.SS);
          LATITUDE.mantissa = ((LATITUDE.MM * 100 / 60 + val) / 100);
          LATITUDE.dec = LATITUDE.DD + LATITUDE.mantissa;
          break;
        case 4:          //   N/S
          LATITUDE.dir = (NMEA.substring(index, pos));

          break;
        case 5:          //   LONGITUDINE 01234.01234

          tmp = (NMEA.substring(index, pos));
          val =  tmp.substring(5, 11).toFloat();
          LONGITUDE.HH = tmp.substring(1, 3).toInt();
          LONGITUDE.MM  = tmp.substring(3, 5).toInt();
          LONGITUDE.SS = (val * 60) ;
          LONGITUDE.ddSS = ((val * 60) - LONGITUDE.SS);
          LONGITUDE.mantissa = ((LONGITUDE.MM * 100 / 60 + val) / 100);
          LONGITUDE.dec = LONGITUDE.HH + LONGITUDE.mantissa;

          break;
        case 6:          //   E/W
          LONGITUDE.dir = (NMEA.substring(index, pos));
          break;
        //      case 7:          //
        //      case 8:          //
        case 9:          //   DATA   061217
          tmp = (NMEA.substring(index, pos));
          TIME.date = tmp.substring(0, 2).toInt();
          TIME.month = tmp.substring(2, 4).toInt();
          TIME.year = tmp.substring(4, 6).toInt();
          break;
          // case 10:
          break;
      }

      index = pos;
      index++;
      field++;
    }
  }
  NMEA = "";
}

void loop() {
  fix();

}


