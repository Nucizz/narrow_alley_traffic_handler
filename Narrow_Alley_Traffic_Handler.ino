// Variabel Pin
const int echoPinA = 3;
const int trigPinA = 2;
const int echoPinB = 4;
const int trigPinB = 5;
const int redPinA = 8;
const int greenPinA = 9;
const int redPinB = 10;
const int greenPinB = 11;
const int buzzerPin = 12;

// Variabel Code
bool stateA = false, stateB = false;

// Variable Default
const int MINIMUM_DISTANCE = 100; // Jarak minimum untuk terdeteksi
const bool BUZZER_USAGE = true; // Meng-aktifkan/non-aktifkan buzzer

void setup() { // Melakukan setup dan start animation
  pinMode(echoPinA, INPUT);
  pinMode(echoPinB, INPUT);
  pinMode(trigPinA, OUTPUT);
  pinMode(trigPinB, OUTPUT);
  pinMode(redPinA, OUTPUT);
  pinMode(greenPinA, OUTPUT);
  pinMode(redPinB, OUTPUT);
  pinMode(greenPinB, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  startAnimation();
}

void loop() {
  // Selalu melakukan reset pada lampu dan melakukan scan setiap 1 detik
  initBlackOut();
  delay(1000);

  // Mengupdate variabel state dari masing - masing sensor jarak
  stateA = getSensorState(trigPinA, echoPinA);
  stateB = getSensorState(trigPinB, echoPinB);

  if(stateA && stateB) { // Jika kedua jalur terdeteksi
    // Memberikan bunyi peringatan dan menyalakan lampu merah selama 3 detik untuk kedua jalur sebagai tanda untuk berhenti secepatnya
    stopFlow(3000);
    // Memberikan jalan pada jalur A diikuti dengan B setelahnya
    giveFlow(greenPinA, redPinB);
    giveFlow(greenPinB, redPinA);
  } else if(stateA) { // Memberhentikan jalur B dan memberikan jalan pada jalur A
    giveFlow(greenPinA, redPinB);
  } else if(stateB) { // Memberhentikan jalur A dan memberikan jalan pada jalur B
    giveFlow(greenPinB, redPinA);
  } else { // Menyalakan lampu hijau berkedip mengikuti loop sebagai tanda aktif dan hati - hati
    digitalWrite(greenPinA, HIGH);
    digitalWrite(greenPinB, HIGH);
    delay(1000);
  }

}

void initBlackOut() { // Mengatur semua perangkat lampu yang ada ke state mati
  digitalWrite(redPinA, LOW);
  digitalWrite(redPinB, LOW);
  digitalWrite(greenPinA, LOW);
  digitalWrite(greenPinB, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}

void warningBuzzer(int repeat) { // Menyalakan buzzer sebanyak yang diminta
  if(BUZZER_USAGE) { // Mengecek apakah buzzer dinyalakan
    if(repeat == 0) { // Jika tidak ada repeat waktu bunyi diperpanjang
      tone(buzzerPin, 3500, 500);
    } else { // Jika ada repeat waktu bunyi diperpendek
      for(int i=0;i<repeat;i++) {
        tone(buzzerPin, 3500, 125);
        delay(275);
      }
    }
  }
}

void stopFlow(int sleep) { // Memberhentikan kedua jalur seketika
  digitalWrite(redPinA, HIGH);
  digitalWrite(redPinB, HIGH);
  warningBuzzer(3);
  delay(sleep);
  digitalWrite(redPinA, LOW);
  digitalWrite(redPinB, LOW);
}

void giveFlow(int greenFlowPin, int redStopPin) { // Memberikan jalan pada salah satu lajur dan mengehentikan yang lain selama 13 + 2 detik
  // Lampu hijau dan merah menyala stabil selama 10 detik menandakan waktu tersisa tersisa banyak 
  warningBuzzer(0);
  digitalWrite(greenFlowPin, HIGH);
  digitalWrite(redStopPin, HIGH);
  delay(10000);

  // Lampu berkedip selama 3 detik menandakan waktu tersisa tersisa sedikit
  for(int i=0;i<3;i++) {
    digitalWrite(greenFlowPin, LOW);
    delay(500);
    warningBuzzer(1);
    digitalWrite(greenFlowPin, HIGH);
    delay(375);
  }

  // Lampu hijau dimatikan dan diberikan treshold selama 2 detik untuk memastikan keamanan
  digitalWrite(greenFlowPin, LOW); 
  delay(2000); 
  digitalWrite(redStopPin, LOW);
}

double calcDistance(long duration) { // Sebuah fungsi untuk melakukan perhitungan jarak menggunakan satuan microsecond
  // 0.034 berasal dari 340 m/s yang dikonversi ke microsecond
  // dibagi 2 untuk mendapatkan jarak sesungguhnya karena gelombang bergerak 2x lipat yaitu pergi-pulang
  return duration*0.034/2;
}

bool getSensorState(int trigPin, int echoPin) { // Mengambil state dari sensor jarak
  // Mengirimkan gelombang ultrasonik selama 10 microsecond dengan interval 2 microsecond dan menyalakan LED_BUILTIN sebagai tanda operasi sensor
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mengecek apakah jarak termasuk ke minimum distance untuk dianggap sebagai sebuah deteksi
  if(calcDistance(pulseIn(echoPin, HIGH)) <= MINIMUM_DISTANCE) {
    return true;
  } else {
    return false;
  }
}

void startAnimation() { // Animasi pembuka saat perangkat dijalankan
  // Variabel untuk bunyi buzzer berupa notes dan durasi per notes
  int melody[] = {4186, 3136, 3520, 0, 4186, 3951, 4698, 4186};
  int duration[] = {300, 300, 300, 500, 300, 300, 300, 300};

  // Melakukan animasi dan bunyi opening
  for (int i=0;i<sizeof(melody)/sizeof(melody[0]);i++) {
    // Jika notes bukan 0 atau delay bunyikan dan nyalakan lampu tertentu 
    if(melody[i]) {
      tone(buzzerPin, melody[i]);

      int lightIndex = i % 4;
      switch(lightIndex){
        case 0:
          digitalWrite(redPinA, HIGH);
          break;
        case 1:
          digitalWrite(redPinB, HIGH);
          break;
        case 2:
          digitalWrite(greenPinA, HIGH);
          break;
        case 3:
          digitalWrite(greenPinB, HIGH);
          break;
      }
    }
    
    // Pastikan semua perangkat mati
    delay(duration[i]);
    noTone(buzzerPin);
    initBlackOut();
  }

  // Mematikan semua komponen untuk bersiap
  initBlackOut();
  noTone(buzzerPin);
}