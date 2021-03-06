/**
 * \file twiciso7816commands.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Twic ISO7816 commands.
 */

#include "../commands/twiciso7816commands.hpp"
#include "logicalaccess/services/accesscontrol/formats/fascn200bitformat.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

#include "logicalaccess/services/accesscontrol/formats/bithelper.hpp"
#include "twicprofile.hpp"


namespace logicalaccess
{
	TwicISO7816Commands::TwicISO7816Commands()
	{

	}

	TwicISO7816Commands::~TwicISO7816Commands()
	{

	}

	void TwicISO7816Commands::selectTWICApplication()
	{
		unsigned char rid[5];
		rid[0] = 0xA0;
		rid[1] = 0x00;
		rid[2] = 0x00;
		rid[3] = 0x03;
		rid[4] = 0x67;

		unsigned char pix[6];
		pix[0] = 0x20;
		pix[1] = 0x00;
		pix[2] = 0x00;
		pix[3] = 0x01;
		pix[4] = 0x81;
		pix[5] = 0x01;

		unsigned char command[9];
		memcpy(command, rid, sizeof(rid));
		memcpy(&command[sizeof(rid)], pix, 4);

		try
		{
			// Non gouvernemental card
			getISO7816ReaderCardAdapter()->sendAPDUCommand(0x00, 0xA4, 0x04, 0x00, sizeof(command), command, sizeof(command));
		}
		catch(std::exception&)
		{
			// Gouvernemental card
			command[sizeof(rid)] = 0x10;
			getISO7816ReaderCardAdapter()->sendAPDUCommand(0x00, 0xA4, 0x04, 0x00, sizeof(command), command, sizeof(command));
		}
	}

	bool TwicISO7816Commands::getTWICData(void* data, size_t& dataLength, int64_t dataObject)
	{
		bool ret = false;
		unsigned char command[6];
		size_t commandlen = 0;
		unsigned char result[4096];
		size_t resultlen = sizeof(result);
		unsigned char le = 0x00;

		//// TWIC Card simulation
		//if (dataObject == 0x5FC104)
		//{
		//	if (data != NULL)
		//	{
		//		if (dataLength <= 59)
		//		{
		//			unsigned char uchui[59];
		//			uchui[0] = 0x53;
		//			uchui[1] = 0x39;
		//			uchui[2] = 0x30; // FASC-N Tag
		//			uchui[3] = 0x19; // FASC-N Length
		//			uchui[4] = 0xD7; // Start FASC-N
		//			uchui[5] = 0x03;
		//			uchui[6] = 0x39;
		//			uchui[7] = 0xDA;
		//			uchui[8] = 0x10;
		//			uchui[9] = 0x0A;
		//			uchui[10] = 0x2C;
		//			uchui[11] = 0x10;
		//			uchui[12] = 0xE7;
		//			uchui[13] = 0x3C;
		//			uchui[14] = 0x0D;
		//			uchui[15] = 0xA1;
		//			uchui[16] = 0x68;
		//			uchui[17] = 0x58;
		//			uchui[18] = 0x21;
		//			uchui[19] = 0x08;
		//			uchui[20] = 0x73;
		//			uchui[21] = 0x36;
		//			uchui[22] = 0x90;
		//			uchui[23] = 0x93;
		//			uchui[24] = 0x87;
		//			uchui[25] = 0x03;
		//			uchui[26] = 0x39;
		//			uchui[27] = 0xA3;
		//			uchui[28] = 0xEB;
		//			uchui[29] = 0x34; // GUID Tag
		//			uchui[30] = 0x10; // GUID Length
		//			uchui[31] = 0x00; // Start GUID
		//			uchui[32] = 0x00;
		//			uchui[33] = 0x00;
		//			uchui[34] = 0x00;
		//			uchui[35] = 0x00;
		//			uchui[36] = 0x00;
		//			uchui[37] = 0x00;
		//			uchui[38] = 0x00;
		//			uchui[39] = 0x00;
		//			uchui[40] = 0x00;
		//			uchui[41] = 0x00;
		//			uchui[42] = 0x00;
		//			uchui[43] = 0x00;
		//			uchui[44] = 0x00;
		//			uchui[45] = 0x00;
		//			uchui[46] = 0x00;
		//			uchui[47] = 0x35; // Expiration Data Tag
		//			uchui[48] = 0x08;
		//			uchui[49] = 0x32;
		//			uchui[50] = 0x30;
		//			uchui[51] = 0x31;
		//			uchui[52] = 0x33;
		//			uchui[53] = 0x30;
		//			uchui[54] = 0x33;
		//			uchui[55] = 0x32;
		//			uchui[56] = 0x33;
		//			uchui[57] = 0xFE;
		//			uchui[58] = 0x00;

		//			memcpy(data, uchui, dataLength);
		//			ret = true;
		//		}
		//	}
		//	else
		//	{
		//		dataLength = 59;
		//	}

		//	return ret;
		//}

		command[commandlen++] = 0x5C;
		command[commandlen++] = 0x03;
		command[commandlen++] = 0xFF & (dataObject >> 16);
		command[commandlen++] = 0xFF & (dataObject >> 8);
		command[commandlen++] = 0xFF & dataObject;
		if (dataLength <= 0xff)
		{
			le = (unsigned char)dataLength;
		}

		getISO7816ReaderCardAdapter()->sendAPDUCommand(0x00, 0xCB, 0x3F, 0xFF, static_cast<unsigned char>(commandlen), command, commandlen, le, result, &resultlen);
		if (data != NULL)
		{
			if (dataLength <= (resultlen-2))
			{			
				memcpy(data, result, dataLength);
				ret = true;
			}
		}
		else
		{
			dataLength = resultlen-2;
		}

		return ret;
	}

	bool TwicISO7816Commands::getUnsignedCardholderUniqueIdentifier(void* data, size_t& dataLength)
	{
		//return getData(data, dataLength, 0x3002);
		return getTWICData(data, dataLength, 0x5FC104);
	}

	bool TwicISO7816Commands::getTWICPrivacyKey(void* data, size_t& dataLength)
	{
		//return getData(data, dataLength, 0x2001);*
		return getTWICData(data, dataLength, 0xDFC101);
	}

	bool TwicISO7816Commands::getCardholderUniqueIdentifier(void* data, size_t& dataLength)
	{
		//return getData(data, dataLength, 0x3000);
		return getTWICData(data, dataLength, 0x5FC102);
	}

	bool TwicISO7816Commands::getCardHolderFingerprints(void* data, size_t& dataLength)
	{
		//return getData(data, dataLength, 0x2003);
		return getTWICData(data, dataLength, 0xDFC103);
	}

	bool TwicISO7816Commands::getSecurityObject(void* data, size_t& dataLength)
	{
		//return getData(data, dataLength, 0x9000);
		return getTWICData(data, dataLength, 0xDFC10F);
	}
}
