#define BETWEEN_TEST_DELAY 0
#define BEFORE_RESET_DELAY 1000

void setRGBTests() {
  
  Serial.println("setRGBTEST");
  // -----------
  String test = "Test device is selected";
  String val = setRGB(DEVICE_TEST, false, 200, 200, 200);
  Serial.print("    ");
  if (val == "Device Test") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  delay(BEFORE_RESET_DELAY);
  // Reset after test
  setRGB(DEVICE_TEST, false, 0, 0, 0);

  delay(BETWEEN_TEST_DELAY);

  // -----------
  test = "Ctrl device is selected";
  val = setRGB(DEVICE_CTRL, false, 200, 200, 200);
  Serial.print("    ");
  if (val == "Device Ctrl") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  delay(BEFORE_RESET_DELAY);
  // Reset after test
  setRGB(DEVICE_TEST, false, 0, 0, 0);
  
  // ----------------------------------------
  test = "Light lock correctly denys entry";
  lightLockEnable = true;
  val = setRGB(DEVICE_CTRL, false, 0, 0, 0);
  Serial.print("    ");
  if (val == "Lights Locked") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  // Reset after test
  lightLockEnable = false;
  setRGB(DEVICE_TEST, false, 0, 0, 0);

  // ----------------------------------------
  test = "Light override bipasses Light Lock";
  lightLockEnable = true;
  val = setRGB(DEVICE_TEST, true, 0, 0, 0);
  Serial.print("    ");
  if (val == "Device Test") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  // Reset after test
  lightLockEnable = false;
  setRGB(DEVICE_TEST, false, 0, 0, 0);

  // ----------------------------------------
  test = "Visual Color Test";
  val = setRGB(DEVICE_TEST, true, 200, 0, 0);
  Serial.print("    ");
  Serial.println("RED: " + test);
  delay(1000);
  val = setRGB(DEVICE_TEST, true, 0, 200, 0);
  Serial.print("    ");
  Serial.println("GREEN: " + test);
  delay(1000);
  val = setRGB(DEVICE_TEST, true, 0, 0, 200);
  Serial.print("    ");
  Serial.println("BLUE: " + test);
  delay(1000);
  // Reset after test
  setRGB(DEVICE_TEST, false, 0, 0, 0);


  Serial.println("");
}

void blackOutTest() {
  Serial.println("blackOutTest");
  // ----------------------------------------
  String test = "Toggle Lights OFF";
  // Before Test
  currentRGB[0] = 0;
  currentRGB[1] = 0;
  currentRGB[2] = 0;
  tempRGB[0] = 0;
  tempRGB[1] = 0;
  tempRGB[2] = 0;
  setRGB(device, true, 200, 200, 200);
  delay(500);
  // Test
  String val = blackOut(true);
  Serial.print("    ");
  if (val == "Blackout true") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  delay(BEFORE_RESET_DELAY);
  // Reset after test
  lightLockEnable = false;
  setRGB(DEVICE_TEST, false, 0, 0, 0);

  // ----------------------------------------
  test = "Toggle Lights On using tempRGB";
  // Before Test
  currentRGB[0] = 0;
  currentRGB[1] = 0;
  currentRGB[2] = 0;
  tempRGB[0] = 200;
  tempRGB[1] = 200;
  tempRGB[2] = 200;
  setRGB(device, true, 0, 0, 0);
  delay(500);
  // Test
  val = blackOut(false);
  Serial.print("    ");
  if (val == "Blackout false") {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
  delay(BEFORE_RESET_DELAY);
  // Reset after test
  lightLockEnable = false;
  setRGB(DEVICE_TEST, false, 0, 0, 0);

  // ----------------------------------------
  test = "Life Lock toggles correctly";
  // Before Test
  currentRGB[0] = 0;
  currentRGB[1] = 0;
  currentRGB[2] = 0;
  tempRGB[0] = 200;
  tempRGB[1] = 200;
  tempRGB[2] = 200;
  setRGB(device, true, 200, 200, 200);
  lightLockEnable = false;
  delay(500);
  // Test
  assertIsTrue(lightLockEnable == false, test);
  delay(1000);
  val = blackOut(true);
  assertIsTrue(lightLockEnable == true, test);
  delay(1000);
  val = blackOut(false);
  assertIsTrue(lightLockEnable == false, test);
  delay(BEFORE_RESET_DELAY);
  // Reset after test
  lightLockEnable = false;
  setRGB(DEVICE_TEST, false, 0, 0, 0);
}

void assertIsTrue(boolean val, String test) {
  Serial.print("    ");
  if (val) {
    Serial.println("Pass - " + test);
  } else {
    Serial.println("Error - " + test + ":Value" + val);
  }
}

