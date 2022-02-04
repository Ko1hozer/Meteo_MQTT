void getSendVitesseVent() {
  // Рассчитываем скорость ветра.

  
  int temps_sec = (millis() - lastSendVent) / 1000;  // Теперь инициализируем lastSendVent.
  lastSendVent = millis();
  if (debug) {
  Serial.print("Temps pour le calcul = "); Serial.print(temps_sec); Serial.println(" sec");
  Serial.print("Nombre de déclenchement = "); Serial.println(anemometreCnt);
  }
  // Рассчитываем скорость ветра.
  float vitesseVent = ((((float)anemometreCnt / 2) / (float)temps_sec) * 10000) / 3600;    // Скорость ветра в км / ч = (количество кругов / время счета в сек) * 2,4
  vitesseVent       = round(vitesseVent * 10) / 10;                             // Преобразуем скорость ветра в 1 знак после запятой.

  // Рассчитываем скорость Порыв ветра.
  float vitesseRafale = ((((float)rafalecnt / 2) / 5) * 10000) / 3600;                     // Скорость ветра в км / ч = (количество кругов / время счета в сек) * 2,4
  vitesseRafale       = round(vitesseRafale * 10) / 10;                         // Преобразуем скорость ветра в 1 знак после запятой.

  // On réinitialise les compteurs.
  anemometreCnt     = 0;                                          // Envoi des données, On réinitialise le compteur de vent à 0.
  anemometreOld     = 0;                                          // Envoi des données, On réinitialise le compteur de mémoire à 0.
  rafalecnt         = 0;                                          // Envoi des données, On réinitialise le compteur de Rafale à 0.

 if (debug) {
  Serial.print("Скорость ветра = "); Serial.println(vitesseVent,1);  // On affiche la vitesse du vent.
  Serial.print("Порыв ветра = "); Serial.println(vitesseRafale,1); 
 }
  client.publish(vitessevent_topic, String(vitesseVent).c_str(), true);
  client.publish(vitesserafale_topic, String(vitesseRafale).c_str(), true);
}

void getRafale() {
  // On calcul le nombre d'impulsion sur les 5 dernières secondes.
  int compteur = anemometreCnt - anemometreOld;
  // On vérifie si la rafale est supérieure à la précédente.
  Serial.print(compteur); Serial.print(" = "); Serial.print(anemometreCnt); Serial.print(" - "); Serial.println(anemometreOld); 
  
  // On stock la nouvelle valeur comme étant l'ancienne pour le prochain traitement.
  anemometreOld = anemometreCnt;
  // On vérifie si la rafale est supérieure à la précédente.
  if (compteur > rafalecnt) {
    // La rafale est supérieure, on enregistre l'information.
    rafalecnt = compteur;
    Serial.print("Nouvelle valeur de rafale : "); Serial.println(rafalecnt);
  }
}

void getdirWind() {
  static String old_wd;
  int sensorValue = analogRead(ventPin);
  
  // Initialisation des varibles utilisées dans cette procédure.
  float dirvent = sensorValue * 3.3;
  wd = "other";
  angw  = 0;

    if(dirvent > 2400 &&  dirvent < 2800 ){
    wd = "N";
    angw  = 0;
  }
    if(dirvent > 1400 &&  dirvent < 1500 ){
    wd = "NNE";
    angw  = 22,5;
  }
    if(dirvent > 1600 &&  dirvent < 1700 ){
    wd = "NE";
    angw  = 45;
  }
    if(dirvent > 300 &&  dirvent < 330 ){
    wd = "ENE";
    angw  = 77,5;
  }
    if(dirvent > 330 &&  dirvent < 400 ){
    wd = "E";
    angw  = 90;
  }
    if(dirvent > 200 &&  dirvent < 280 ){
    wd = "ESE";
    angw  = 112,5;
  }
    if(dirvent > 600 &&  dirvent < 700 ){
    wd = "SE";
    angw  = 135;
  }
    if(dirvent > 400 &&  dirvent < 500 ){
    wd = "SSE";
    angw  = 157,5;
  }
    if(dirvent > 980 &&  dirvent < 1100 ){
    wd = "S";
    angw  = 180;
  }
    if(dirvent > 850 &&  dirvent < 900 ){
    wd = "SSW";
    angw  = 202,5;
  }
    if(dirvent > 2150 &&  dirvent < 2250 ){
    wd = "SW";
    angw  = 225;
  }
    if(dirvent > 2070 &&  dirvent < 2150 ){
    wd = "WSW";
    angw  = 247,5;
  }
  if(dirvent > 3150 &&  dirvent < 3250 ){
    wd = "W";
    angw  = 270;
  }
    if(dirvent > 2860 &&  dirvent < 2950 ){
    wd = "WNW";
    angw  = 292,5;
  }
  if(dirvent > 3040 &&  dirvent < 3146 ){
    wd = "NW";
    angw  = 315;
  }
  if(wd == "other") {
    wd = old_wd;
  } else {
    old_wd = wd;
  }
  if (debug) {
    Serial.print("Направление ветра : "); Serial.println(wd);
    Serial.print("Старое направление ветра : "); Serial.println(old_wd);
    Serial.print("Направление в градусах : "); Serial.println(angw);
    Serial.print("коэфицент : "); Serial.println(dirvent);
  }
    client.publish(windangw_topic, String(angw).c_str(), true);
    client.publish(dirwind_topic, String(wd).c_str(), true);
}

void getBME280Values() {
    float newTemp = bme.readTemperature();
    float newHum = bme.readHumidity();
    float pressurePascals = bme.readPressure();
    float newPres  = (pressurePascals / 100.0) * 0.750062; //переводим в милиметры трутного столба
    
    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      if (debug) {
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      }
      client.publish(temperature_topic, String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      if (debug) {
      Serial.print("New Humidity:");
      Serial.println(String(hum).c_str());
      }
      client.publish(humidity_topic, String(hum).c_str(), true);
    }

    if (checkBound(newPres, pres, diff)) {
      pres = newPres;
      if (debug) {
      Serial.print("New Pressure:");
      Serial.println(String(pres).c_str());
      }
      client.publish(pressure_topic, String(pres).c_str(), true);
    }  
}
