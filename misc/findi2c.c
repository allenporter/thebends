// gcc -o findi2c findi2c.c -framework CoreFoundation -framework IOKit
#include <string.h>
#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
//#include <IOI2C/IOI2CDefs.h>

#define DLOG printf

CFArrayRef findI2CDevices(void)
{
	kern_return_t			status;
	io_iterator_t			iter;
	io_registry_entry_t		next, nub, bus;
	CFMutableArrayRef		array;
	CFMutableDictionaryRef	dict;
	char					path[8*1024];
	CFStringRef				pathKey;
	CFStringRef				compatibleKey;
	CFStringRef				cfStr;
	CFTypeRef				cfRef;

	DLOG("findI2CDevices\n");
	// Get an iterator of all IOI2CDevice matches.
	status = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("IOI2CDevice"), &iter);
	if ((status != kIOReturnSuccess) || (iter == 0))
	{
		DLOG("findI2CDevices found none\n");
		return 0;
	}

	pathKey = CFStringCreateWithCString(NULL, "path", kCFStringEncodingMacRoman);
	compatibleKey = CFStringCreateWithCString(NULL, "compatible", kCFStringEncodingMacRoman);
	array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);

	if (pathKey && compatibleKey && array)
	{
		DLOG("findI2CDevices searching...\n");
		while (next = IOIteratorNext(iter))
		{
			DLOG("findI2CDevices next\n");

			if (0 == (dict = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks)))
			{
				DLOG("findI2CDevices CFDict, ain't!\n");
				break;
			}

			if (kIOReturnSuccess == (status = IORegistryEntryGetPath(next, kIOServicePlane, path)))
			{
				DLOG("findI2CDevices got path:\n\"%s\"\n",path);

				if (cfStr = CFStringCreateWithCString(NULL, path, kCFStringEncodingMacRoman))
				{
					DLOG("findI2CDevices adding path\n");
					CFDictionaryAddValue(dict, pathKey, cfStr), 
					CFRelease(cfStr);
				}

				DLOG("findI2CDevices adding class key\n");
				CFDictionaryAddValue(dict, CFSTR("IOClass"), CFSTR("IOI2CDevice"));

				if (kIOReturnSuccess == (status = IORegistryEntryGetParentEntry(next, kIOServicePlane, &nub)))
				{
					if (cfRef = IORegistryEntryCreateCFProperty(nub, compatibleKey, NULL, 0))
					{
						DLOG("findI2CDevices got compatible\n");
						CFDictionaryAddValue(dict, compatibleKey, cfRef);
						CFRelease(cfRef);
					}

					if (cfRef = IORegistryEntryCreateCFProperty(nub, CFSTR("reg"), NULL, 0))
					{
						DLOG("findI2CDevices got address\n");
						CFDictionaryAddValue(dict, CFSTR("address"), cfRef);
						CFRelease(cfRef);
					}

					if (kIOReturnSuccess == (status = IORegistryEntryGetParentEntry(nub, kIODeviceTreePlane, &bus)))
					{
						// The device parent can be a single-bus controller; 
						// in which case the controller may use the "reg" property for iomapping
						// in which case the bus id will be in the "AAPL,bus-id" property...
						if (cfRef = IORegistryEntryCreateCFProperty(bus, CFSTR("AAPL,i2c-bus"), NULL, 0))
						{
							DLOG("findI2CDevices got bus by \"AAPL,i2c-bus\"\n");
							CFDictionaryAddValue(dict, CFSTR("bus"), cfRef);
							CFRelease(cfRef);
						}
						else
						if (cfRef = IORegistryEntryCreateCFProperty(bus, CFSTR("reg"), NULL, 0))
						{
							DLOG("findI2CDevices got bus by \"reg\"\n");
							CFDictionaryAddValue(dict, CFSTR("bus"), cfRef);
							CFRelease(cfRef);
						}
						IOObjectRelease(bus);
					}

					IOObjectRelease(nub);
				}

				DLOG("findI2CDevices got past compatible\n");

				CFArrayAppendValue(array, dict);
				DLOG("findI2CDevices got past array dict\n");
			}
		}
		DLOG("findI2CDevices iter done\n");
	}

	DLOG("findI2CDevices release iter\n");
	IOObjectRelease(iter);
	if (pathKey)
		CFRelease(pathKey);
	if (compatibleKey)
		CFRelease(compatibleKey);
	if (array)
	{
		if (CFArrayGetCount(array) > 0)
		{
			DLOG("findI2CDevices found devices\n");
			return array;
		}

		CFRelease(array);
	}
	return 0;
}

void dump_dictionary(CFDictionaryRef attributesDict) {
  CFStringRef * attributeKeys;
  CFStringRef * attributeValues;
  CFIndex attributeCount;

  attributeCount = CFDictionaryGetCount(attributesDict);
  attributeKeys = (CFStringRef *)malloc(attributeCount * sizeof(CFStringRef));
  attributeValues = (CFStringRef *)malloc(attributeCount * sizeof(CFStringRef));

  CFDictionaryGetKeysAndValues(attributesDict, (const void **)attributeKeys,
    (const void **)attributeValues);

  CFIndex j;
  for (j=0; j < attributeCount; j++) {
    CFStringRef key;
    CFStringRef value;
    key = attributeKeys[j];
    value = attributeValues[j];

    printf("[%s]=[%s]\n", key, value);
  }
}

int main(int argc, char * argv) {
  CFArrayRef arr = findI2CDevices();
  if (arr == NULL) {
    return;
  }

  CFIndex num_devices = CFArrayGetCount(arr);
  CFIndex i;
  for (i = 0; i < num_devices; ++i) {
    printf("===\n");
    CFDictionaryRef dict = (CFDictionaryRef)CFArrayGetValueAtIndex(arr, i); 
    dump_dictionary(dict); 
  }
  
}
