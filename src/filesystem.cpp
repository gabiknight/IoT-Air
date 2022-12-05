#include <header.h>

/**
 * @brief Initialize LittleFS
 * 
 * @param fs 
 */
void initFS(fs::FS &fs)
{
  if (!fs.begin())
  {
    PRINTF("An error has occurred while mounting LittleFS");
  }
  else
  {
    PRINTF("LittleFS mounted successfully");
  }
}

/**
 * @brief Read File from LittleFS
 * 
 * @param fs 
 * @param path 
 * @return String 
 */
String readFile(fs::FS &fs, const char *path)
{
  PRINTF("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    PRINTF("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available())
  {
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

/**
 * @brief Write file to LittleFS
 * 
 * @param fs 
 * @param path 
 * @param message 
 */
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  PRINTF("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if (!file)
  {
    PRINTF("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    PRINTF("- file written");
  }
  else
  {
    PRINTF("- frite failed");
  }
  file.close();
}
