#ifndef HID_SERVICE_BASE_H_
#define HID_SERVICE_BASE_H_

#include "mbed.h"

#include "ble/BLE.h"
#include "HID_types.h"

#define BLE_UUID_DESCRIPTOR_REPORT_REFERENCE 0x2908

typedef const uint8_t report_map_t[];
typedef const uint8_t * report_t;

typedef struct {
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  flags;
} HID_information_t;

enum ReportType {
    INPUT_REPORT    = 0x1,
    OUTPUT_REPORT   = 0x2,
    FEATURE_REPORT  = 0x3,
};

enum ProtocolMode {
    BOOT_PROTOCOL   = 0x0,
    REPORT_PROTOCOL = 0x1,
};

typedef struct {
    uint8_t ID;
    uint8_t type;
} report_reference_t;


class HIDServiceBase {
public:
    /**
     *  Constructor
     *
     *  @param _ble
     *         BLE object to add this service to
     *  @param reportMap
     *         Byte array representing the input/output report formats. In USB HID jargon, it
     *         is called "HID report descriptor".
     *  @param reportMapLength 
     *         Size of the reportMap array
     *  @param outputReportLength
     *         Maximum length of a sent report (up to 64 bytes) (default: 64 bytes)
     *  @param inputReportLength
     *         Maximum length of a received report (up to 64 bytes) (default: 64 bytes)
     *  @param inputReportTickerDelay
     *         Delay between input report notifications, in ms. Acceptable values depend directly on
     *         GAP's connInterval parameter, so it shouldn't be less than 12ms
     *         Preferred GAP connection interval is set after this value, in order to send
     *         notifications as quick as possible: minimum connection interval will be set to
     *         (inputReportTickerDelay / 2)
     */
    HIDServiceBase(BLE &_ble,
                   report_map_t reportMap,
                   uint8_t reportMapLength,
                   report_t inputReport,
                   report_t outputReport,
                   report_t featureReport,
                   uint8_t inputReportLength = 0,
                   uint8_t outputReportLength = 0,
                   uint8_t featureReportLength = 0);

    /**
     *  Send Report
     *
     *  @param report   Report to send. Must be of size @ref inputReportLength
     *  @return         The write status
     *
     *  @note Don't call send() directly for multiple reports! Use reportTicker for that, in order
     *  to avoid overloading the BLE stack, and let it handle events between each report.
     */
    virtual ble_error_t send(const report_t report);

    /**
     *  Read Report
     *
     *  @param report   Report to fill. Must be of size @ref outputReportLength
     *  @return         The read status
     */
    virtual ble_error_t read(report_t report);


protected:
    /**
     * Called by BLE API when data has been successfully sent.
     *
     * @param count     Number of reports sent
     *
     * @note Subclasses can override this to avoid starting the report ticker when there is nothing
     * to send
     */

    /**
     * Create the Gatt descriptor for a report characteristic
     */
    GattAttribute** inputReportDescriptors();
    GattAttribute** outputReportDescriptors();
    GattAttribute** featureReportDescriptors();

    /**
     * Create the HID information structure
     */
    HID_information_t* HIDInformation();

protected:
    BLE &ble;

    int reportMapLength;

    report_t inputReport;
    report_t outputReport;
    report_t featureReport;

    uint8_t inputReportLength;
    uint8_t outputReportLength;
    uint8_t featureReportLength;

    uint8_t controlPointCommand;
    uint8_t protocolMode;

    report_reference_t inputReportReferenceData;
    report_reference_t outputReportReferenceData;
    report_reference_t featureReportReferenceData;

    GattAttribute inputReportReferenceDescriptor;
    GattAttribute outputReportReferenceDescriptor;
    GattAttribute featureReportReferenceDescriptor;

    // Optional gatt characteristics:
    GattCharacteristic protocolModeCharacteristic;

    // Report characteristics (each sort of optional)
    GattCharacteristic inputReportCharacteristic;
    GattCharacteristic outputReportCharacteristic;
    GattCharacteristic featureReportCharacteristic;

    // Required gatt characteristics: Report Map, Information, Control Point
    GattCharacteristic reportMapCharacteristic;
    ReadOnlyGattCharacteristic<HID_information_t> HIDInformationCharacteristic;
    GattCharacteristic HIDControlPointCharacteristic;

};

#endif /* !HID_SERVICE_BASE_H_ */

