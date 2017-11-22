#include "Headers/GATT_SPECIFICATIONS.h"

namespace BLEGateway {


std::string GetGATTDescriptions(int code) {

	switch (code) {

	case 0x2800:
		return "GATT Primary Service Declaration";
	case 0x2801:
		return "GATT Secondary Service Declaration";
	case 0x2802:
		return "GATT Include Declaration";
	case 0x2803:
		return "GATT Characteristic Declaration";
	case 0x2900:
		return "Characteristic Extended Properties";
	case 0x2901:
		return "Characteristic User Description";
	case 0x2902:
		return "Client Characteristic Configuration";
	case 0x2903:
		return "Server Characteristic Configuration";
	case 0x2904:
		return "Characteristic Presentation Format";
	case 0x2905:
		return "Characteristic Aggregate Format";
	case 0x2906:
		return "Valid Range";
	case 0x2907:
		return "External Report Reference";
	case 0x2908:
		return "Report Reference";
	case 0x290B:
		return "Environmental Sensing Configuration	";
	case 0x290C:
		return "Environmental Sensing Measurement";
	case 0x290D:
		return "Environmental Sensing Trigger Setting	";
	case 0x2909:
		return "Number of Digitals";
	case 0x290A:
		return "Value Trigger Setting";
	case 0x290E:
		return "Time Trigger Setting";
	case 0x2A7E:
		return "Aerobic Heart Rate Lower Limit";
	case 0x2A84:
		return "Aerobic Heart Rate Upper Limit";
	case 0x2A7F:
		return "Aerobic Threshold";
	case 0x2A80:
		return "Age";
	case 0x2A43:
		return "Alert Category ID";
	case 0x2A42:
		return "Alert Category ID Bit Mask";
	case 0x2A06:
		return "Alert Level";
	case 0x2A44:
		return "Alert Notification Control Point";
	case 0x2A3F:
		return "Alert Status";
	case 0x2AB3:
		return "Altitude";
	case 0x2A81:
		return "Anaerobic Heart Rate Lower Limit";
	case 0x2A82:
		return "Anaerobic Heart Rate Upper Limit";
	case 0x2A83:
		return "Anaerobic Threshold";
	case 0x2A73:
		return "Apparent Wind Direction ";
	case 0x2A72:
		return "Apparent Wind Speed";
	case 0x2A01:
		return "Appearance";
	case 0x2AA3:
		return "Barometric Pressure Trend";
	case 0x2A19:
		return "Battery Level";
	case 0x2A49:
		return "Blood Pressure Feature";
	case 0x2A35:
		return "Blood Pressure Measurement";
	case 0x2A9B:
		return "Body Composition Feature";
	case 0x2A9C:
		return "Body Composition Measurement";
	case 0x2A38:
		return "Body Sensor Location";
	case 0x2AA4:
		return "Bond Management Control Point";
	case 0x2AA5:
		return "Bond Management Feature";
	case 0x2A22:
		return "Boot Keyboard Input Report";
	case 0x2A32:
		return "Boot Keyboard Output Report";
	case 0x2A33:
		return "Boot Mouse Input Report";
	case 0x2AA6:
		return "Central Address Resolution";
	case 0x2AA8:
		return "CGM Feature";
	case 0x2AA7:
		return "CGM Measurement";
	case 0x2AAB:
		return "CGM Session Run Time";
	case 0x2AAA:
		return "CGM Session Start Time";
	case 0x2AAC:
		return "CGM Specific Ops Control Point";
	case 0x2AA9:
		return "CGM Status";
	case 0x2A5C:
		return "CSC Feature";
	case 0x2A5B:
		return "CSC Measurement";
	case 0x2A2B:
		return "Current Time";
	case 0x2A66:
		return "Cycling Power Control Point";
	case 0x2A65:
		return "Cycling Power Feature";
	case 0x2A63:
		return "Cycling Power Measurement";
	case 0x2A64:
		return "Cycling Power Vector";
	case 0x2A99:
		return "Database Change Increment";
	case 0x2A85:
		return "Date of Birth";
	case 0x2A86:
		return "Date of Threshold Assessment ";
	case 0x2A08:
		return "Date Time";
	case 0x2A0A:
		return "Day Date Time";
	case 0x2A09:
		return "Day of Week";
	case 0x2A7D:
		return "Descriptor Value Changed";
	case 0x2A00:
		return "Device Name";
	case 0x2A7B:
		return "Dew Point";
	case 0x2A0D:
		return "DST Offset";
	case 0x2A6C:
		return "Elevation";
	case 0x2A87:
		return "Email Address";
	case 0x2A0C:
		return "Exact Time 256";
	case 0x2A88:
		return "Fat Burn Heart Rate Lower Limit";
	case 0x2A89:
		return "Fat Burn Heart Rate Upper Limit";
	case 0x2A26:
		return "Firmware Revision String";
	case 0x2A8A:
		return "First Name";
	case 0x2A8B:
		return "Five Zone Heart Rate Limits";
	case 0x2AB2:
		return "Floor Number";
	case 0x2A8C:
		return "Gender";
	case 0x2A51:
		return "Glucose Feature";
	case 0x2A18:
		return "Glucose Measurement";
	case 0x2A34:
		return "Glucose Measurement Context";
	case 0x2A74:
		return "Gust Factor";
	case 0x2A27:
		return "Hardware Revision String";
	case 0x2A39:
		return "Heart Rate Control Point";
	case 0x2A8D:
		return "Heart Rate Max";
	case 0x2A37:
		return "Heart Rate Measurement";
	case 0x2A7A:
		return "Heat Index";
	case 0x2A8E:
		return "Height";
	case 0x2A4C:
		return "HID Control Point";
	case 0x2A4A:
		return "HID Information";
	case 0x2A8F:
		return "Hip Circumference";
	case 0x2A6F:
		return "Humidity";
	case 0x2A2A:
		return "IEEE 11073-20601 Regulatory Certification Data List";
	case 0x2AAD:
		return "Indoor Positioning Configuration";
	case 0x2A36:
		return "Intermediate Cuff Pressure";
	case 0x2A1E:
		return "Intermediate Temperature";
	case 0x2A77:
		return "Irradiance";
	case 0x2AA2:
		return "Language";
	case 0x2A90:
		return "Last Name";
	case 0x2AAE:
		return "Latitude";
	case 0x2A6B:
		return "LN Control Pointt";
	case 0x2A6A:
		return "LN Feature";
	case 0x2AB1:
		return "Local East Coordinate";
	case 0x2AB0:
		return "Local North Coordinate";
	case 0x2A0F:
		return "Local Time Information";
	case 0x2A67:
		return "Location and Speed";
	case 0x2AB5:
		return "Location Name";
	case 0x2AAF:
		return "Longitude";
	case 0x2A2C:
		return "Magnetic Declination";
	case 0x2AA0:
		return "Magnetic Flux Density - 2D";
	case 0x2AA1:
		return "Magnetic Flux Density - 3D";
	case 0x2A29:
		return "Manufacturer Name String";
	case 0x2A91:
		return "Maximum Recommended Heart Rate";
	case 0x2A21:
		return "Measurement Interval";
	case 0x2A24:
		return "Model Number String";
	case 0x2A68:
		return "Navigation";
	case 0x2A46:
		return "New Alert";
	case 0x2A04:
		return "Peripheral Preferred Connection Parameters";
	case 0x2A02:
		return "Peripheral Privacy Flag";
	case 0x2A50:
		return "PnP ID";
	case 0x2A75:
		return "Pollen Concentration";
	case 0x2A69:
		return "Position Quality";
	case 0x2A6D:
		return "Pressure";
	case 0x2A4E:
		return "Protocol Mode";
	case 0x2A78:
		return "Rainfall";
	case 0x2A03:
		return "Reconnection Address";
	case 0x2A52:
		return "Record Access Control Point";
	case 0x2A14:
		return "Reference Time Information";
	case 0x2A4D:
		return "Report";
	case 0x2A4B:
		return "Report Map";
	case 0x2A92:
		return "Resting Heart Rate";
	case 0x2A40:
		return "Ringer Control Point";
	case 0x2A41:
		return "Ringer Setting";
	case 0x2A54:
		return "RSC Feature";
	case 0x2A53:
		return "RSC Measurement";
	case 0x2A55:
		return "SC Control Point";
	case 0x2A4F:
		return "Scan Interval Window";
	case 0x2A31:
		return "Scan Refresh";
	case 0x2A5D:
		return "Sensor Location";
	case 0x2A25:
		return "Serial Number String";
	case 0x2A05:
		return "Service Changed";
	case 0x2A28:
		return "Software Revision String";
	case 0x2A93:
		return "Sport Type for Aerobic and Anaerobic Thresholds";
	case 0x2A47:
		return "Supported New Alert Category";
	case 0x2A48:
		return "Supported Unread Alert Category";
	case 0x2A23:
		return "System ID";
	case 0x2A6E:
		return "Temperature";
	case 0x2A1C:
		return "Temperature Measurement";
	case 0x2A1D:
		return "Temperature Type";
	case 0x2A94:
		return "Three Zone Heart Rate Limits";
	case 0x2A12:
		return "Time Accuracy";
	case 0x2A13:
		return "Time Source";
	case 0x2A16:
		return "Time Update Control Point";
	case 0x2A17:
		return "Time Update State";
	case 0x2A11:
		return "Time with DST";
	case 0x2A0E:
		return "Time Zone";
	case 0x2A71:
		return "True Wind Direction";
	case 0x2A70:
		return "True Wind Speed";
	case 0x2A95:
		return "Two Zone Heart Rate Limit";
	case 0x2A07:
		return "Tx Power Level";
	case 0x2AB4:
		return "Uncertainty";
	case 0x2A45:
		return "Unread Alert Status";
	case 0x2A9F:
		return "User Control Point";
	case 0x2A9A:
		return "User Index";
	case 0x2A76:
		return "UV Index";
	case 0x2A96:
		return "VO2 Max";
	case 0x2A97:
		return "Waist Circumference ";
	case 0x2A98:
		return "Weight";
	case 0x2A9D:
		return "Weight Measurement";
	case 0x2A9E:
		return "Weight Scale Feature";
	case 0x2A79:
		return "Wind Chill";
	case 0x2A5A:
		return "Aggregate";
	case 0x2A58:
		return "Analog";
	case 0x2A56:
		return "Digital";

	case 0x1811:
		return "Alert Notification Service";
	case 0x180F:
		return "Battery Service";
	case 0x1810:
		return "Blood Pressure";
	case 0x181B:
		return "Body Composition";
	case 0x181E:
		return "Bond Management";
	case 0x181F:
		return "Continuous Glucose Monitoring";
	case 0x1805:
		return "Current Time Service";
	case 0x1818:
		return "Cycling Power";
	case 0x1816:
		return "Cycling Speed and Cadence";
	case 0x180A:
		return "Device Information";
	case 0x181A:
		return "Environmental Sensing";
	case 0x1800:
		return "Generic Access";
	case 0x1801:
		return "Generic Attribute";
	case 0x1808:
		return "Glucose";
	case 0x1809:
		return "Health Thermometer";
	case 0x180D:
		return "Heart Rate";
	case 0x1812:
		return "Human Interface Device";
	case 0x1802:
		return "Immediate Alert";
	case 0x1821:
		return "Indoor Positioning";
	case 0x1820:
		return "Internet Protocol Support";
	case 0x1803:
		return "Link Loss";
	case 0x1819:
		return "Location and Navigation";
	case 0x1807:
		return "Next DST Change Service";
	case 0x180E:
		return "Phone Alert Status Service";
	case 0x1806:
		return "Reference Time Update Service";
	case 0x1814:
		return "Running Speed and Cadence";
	case 0x1813:
		return "Scan Parameters";
	case 0x1804:
		return "Tx Power";
	case 0x181C:
		return "User Data";
	case 0x181D:
		return "Weight Scale";
	case 0x1815:
		return "Automation IO";
	default:
		return "";


	}
}
}
