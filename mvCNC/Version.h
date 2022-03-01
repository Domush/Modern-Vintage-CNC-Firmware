/**
 * Modern Vintage CNC Firmware
*/
#pragma once

////////////////////////////
// VENDOR VERSION EXAMPLE //
////////////////////////////

/**
 * mvCNC release version identifier
 */
#define SHORT_BUILD_VERSION "mvCNC v0.1.0"

/**
 * Verbose version identifier which should contain a reference to the location
 * from where the binary was downloaded or the source code was compiled.
 */
//#define DETAILED_BUILD_VERSION SHORT_BUILD_VERSION

/**
 * The STRING_DISTRIBUTION_DATE represents when the binary file was built,
 * here we define this default string as the date where the latest release
 * version was tagged.
 */
//#define STRING_DISTRIBUTION_DATE "2022-02-08"

/**
 * Defines a generic cnc name to be output to the LCD after booting mvCNC.
 */
#define MACHINE_NAME "Modern Vintage CNC"

/**
 * The SOURCE_CODE_URL is the location where users will find the mvCNC Source
 * Code which is installed on the device. In most cases —unless the manufacturer
 * has a distinct Github fork— the Source Code URL should just be the main
 * mvCNC repository.
 */
#define SOURCE_CODE_URL "https://github.com/Domush/Modern-Vintage-CNC-Firmware.git"

/**
 * Default generic cnc UUID.
 */
//#define DEFAULT_MACHINE_UUID "cede2a2f-41a2-4748-9b12-c55c62f367ff"

/**
 * The WEBSITE_URL is the location where users can get more information such as
 * documentation about a specific mvCNC release.
 */
//#define WEBSITE_URL "mvcncfw.org"

/**
 * Set the vendor info the serial USB interface, if changable
 * Currently only supported by DUE platform
 */
//#define USB_DEVICE_VENDOR_ID           0x0000
//#define USB_DEVICE_PRODUCT_ID          0x0000
//#define USB_DEVICE_MANUFACTURE_NAME    WEBSITE_URL
