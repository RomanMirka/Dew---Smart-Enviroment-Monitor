const uint8_t* select_bitmap() {
  if (!isFirstReadDone) return nullptr;

  if (screen_mode == 0) { //temperature mode
    if      (temperature < TEMP_LOW_MAX)  return bmp_temp_low;
    else if (temperature > TEMP_HIGH_MIN) return bmp_temp_high;
    else                                  return bmp_temp_normal;
  } else if (screen_mode == 1) { //humidity mode
    if      (humidity < HUM_LOW_MAX)  return bmp_hum_low;
    else if (humidity > HUM_HIGH_MIN) return bmp_hum_high;
    else                              return bmp_hum_normal;
  }
  return nullptr;
}

const uint8_t* showSleepBitmap() {
 int random_num = random(1, 7);
  switch (random_num) {
    case 1:
    return bmp_sleep_1;
    case 2:
    return bmp_sleep_2;
    case 3:
    return bmp_sleep_3;
    case 4:
    return bmp_sleep_4;
    case 5:
    return bmp_sleep_5;
    case 6:
    return bmp_sleep_6;
  }
  return nullptr;
}