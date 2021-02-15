// *** EEPROM CODE: https://github.com/jwrw/ESP_EEPROM/blob/master/examples/ESP_EEPROM_Use/ESP_EEPROM_Use.ino
#include <ESP_EEPROM.h>

void get_settings_from_eeprom(){
  // *** EEPROM CODE
  // The begin() call will find the data previously saved in EEPROM if the same size
  // as was previously committed. If the size is different then the EEEPROM data is cleared. 
  // Note that this is not made permanent until you call commit();
  EEPROM.begin(sizeof(SettingsStruct));

  // Check if the EEPROM contains valid data from another run
  // If so, overwrite the 'default' values set up in our struct
  if(EEPROM.percentUsed()>=0) {
    EEPROM.get(0, settings);
    Serial.println("EEPROM has data from a previous run.");
    Serial.print(EEPROM.percentUsed());
    Serial.println("% of ESP flash space currently used");
  } else {
    Serial.println("EEPROM size changed - EEPROM data zeroed - commit() to make permanent"); 
    // set the EEPROM data ready for writing
    EEPROM.put(0, settings_default);
    settings = settings_default;
    // write the data to EEPROM
    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
  }
}

void put_settings_to_eeprom(){
  // *** EEPROM CODE
  // The begin() call will find the data previously saved in EEPROM if the same size
  // as was previously committed. If the size is different then the EEEPROM data is cleared. 
  // Note that this is not made permanent until you call commit();
  EEPROM.begin(sizeof(SettingsStruct));
  EEPROM.put(0, settings);
  // write the data to EEPROM
  boolean ok = EEPROM.commit();
  // if data new data is identical to what is already stored, commit() will return False
  Serial.println((ok) ? "Commit OK" : "Commit failed (maybe data did not change?");
}

void setup_eeprom(){
  get_settings_from_eeprom();
  Serial.print("EEPROM data read, settings.backgroundcolor=");
  Serial.println(settings.backgroundcolor);
}
