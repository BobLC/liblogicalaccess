/**
 * \file mifareaccessinfo.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Access informations for mifare card.
 */

#include "mifareaccessinfo.hpp"
#include "mifarechip.hpp"

namespace logicalaccess
{
	size_t MifareAccessInfo::SectorAccessBits::toArray(void* buf, size_t buflen) const
	{
		if (buflen < 3)
		{
			THROW_EXCEPTION_WITH_LOG(std::invalid_argument, "The buffer is too short.");
		}

		memset(buf, 0x00, 3);

		unsigned char* data = reinterpret_cast<unsigned char*>(buf);

		data[1] = (d_sector_trailer_access_bits.c1 << 7) | (d_data_blocks_access_bits[2].c1 << 6) | (d_data_blocks_access_bits[1].c1 << 5) | (d_data_blocks_access_bits[0].c1 << 4);
		data[2] = (d_sector_trailer_access_bits.c3 << 7) | (d_data_blocks_access_bits[2].c3 << 6) | (d_data_blocks_access_bits[1].c3 << 5) | (d_data_blocks_access_bits[0].c3 << 4);
		data[2] |= (d_sector_trailer_access_bits.c2 << 3) | (d_data_blocks_access_bits[2].c2 << 2) | (d_data_blocks_access_bits[1].c2 << 1) | (d_data_blocks_access_bits[0].c2 << 0);
		data[0] = ((~data[2]) << 4) & 0xF0;
		data[0] |= ((~data[1]) >> 4) & 0x0F;
		data[1] |= ((~data[2]) >> 4) & 0x0F;

		return 3;
	}

	bool MifareAccessInfo::SectorAccessBits::fromArray(const void* buf, size_t buflen)
	{
		if (buflen < 3)
		{
			THROW_EXCEPTION_WITH_LOG(std::invalid_argument, "The buffer is too short.");
		}

		SectorAccessBits sab;

		const unsigned char* data = reinterpret_cast<const unsigned char*>(buf);

		sab.d_sector_trailer_access_bits.c1 = ((data[1] & 0x80) != 0);
		sab.d_sector_trailer_access_bits.c2 = ((data[2] & 0x08) != 0);
		sab.d_sector_trailer_access_bits.c3 = ((data[2] & 0x80) != 0);
		sab.d_data_blocks_access_bits[2].c1 = ((data[1] & 0x40) != 0);
		sab.d_data_blocks_access_bits[2].c2 = ((data[2] & 0x04) != 0);
		sab.d_data_blocks_access_bits[2].c3 = ((data[2] & 0x40) != 0);
		sab.d_data_blocks_access_bits[1].c1 = ((data[1] & 0x20) != 0);
		sab.d_data_blocks_access_bits[1].c2 = ((data[2] & 0x02) != 0);
		sab.d_data_blocks_access_bits[1].c3 = ((data[2] & 0x20) != 0);
		sab.d_data_blocks_access_bits[0].c1 = ((data[1] & 0x10) != 0);
		sab.d_data_blocks_access_bits[0].c2 = ((data[2] & 0x01) != 0);
		sab.d_data_blocks_access_bits[0].c3 = ((data[2] & 0x10) != 0);

		unsigned char test[3];

		if (sab.toArray(test, 3) > 0)
		{
			if (memcmp(test, buf, 3) == 0)
			{
				operator=(sab);

				return true;
			}
		}

		return false;
	}

	void MifareAccessInfo::SectorAccessBits::setTransportConfiguration()
	{
		d_sector_trailer_access_bits.c1 = false;
		d_sector_trailer_access_bits.c2 = false;
		d_sector_trailer_access_bits.c3 = true;

		for (unsigned int i = 0; i < 3; ++i)
		{
			d_data_blocks_access_bits[i].c1 = false;
			d_data_blocks_access_bits[i].c2 = false;
			d_data_blocks_access_bits[i].c3 = false;
		}
	}

	void MifareAccessInfo::SectorAccessBits::setAReadBWriteConfiguration()
	{
		d_sector_trailer_access_bits.c1 = false;
		d_sector_trailer_access_bits.c2 = true;
		d_sector_trailer_access_bits.c3 = true;

		for (unsigned int i = 0; i < 3; ++i)
		{
			d_data_blocks_access_bits[i].c1 = true;
			d_data_blocks_access_bits[i].c2 = false;
			d_data_blocks_access_bits[i].c3 = false;
		}
	}

	MifareAccessInfo::MifareAccessInfo()
	{
		keyA.reset(new MifareKey());
		keyA->fromString("FF FF FF FF FF FF");
		keyB.reset(new MifareKey());
		keyB->fromString("");
		madKeyA.reset(new MifareKey());
		madKeyA->fromString("FF FF FF FF FF FF");
		madKeyB.reset(new MifareKey());
		madKeyB->fromString("");
		useMAD = false;
		sab.setTransportConfiguration();
		gpb = 0x00;
		madGPB = 0xC3; // DA: 1, MA: 1, RFU, ADV: 01
	}

	MifareAccessInfo::~MifareAccessInfo()
	{		
	}

	std::string MifareAccessInfo::getCardType() const
	{
		return CHIP_MIFARE;
	}

	void MifareAccessInfo::generateInfos()
	{		
		keyA->fromString(generateSimpleKey(0, MIFARE_KEY_SIZE));
		keyB->fromString(generateSimpleKey(1000, MIFARE_KEY_SIZE));

		sab.setAReadBWriteConfiguration();
	}

	void MifareAccessInfo::generateInfos(const string& csn)
	{
		long lcsn = atol(csn.c_str());

		keyA->fromString(generateSimpleKey(lcsn, MIFARE_KEY_SIZE));
		keyB->fromString(generateSimpleKey(lcsn + 1000, MIFARE_KEY_SIZE));

		sab.setAReadBWriteConfiguration();
	}

	std::vector<unsigned char> MifareAccessInfo::getLinearData() const
	{
		std::vector<unsigned char> data;
		
		data.push_back(static_cast<unsigned char>(keyA->isEmpty() ? 0 : 1));

		if (!keyA->isEmpty())
		{
			unsigned char* keydata = keyA->getData();
			data.insert(data.end(), keydata, keydata + keyA->getLength());
		}
		else
		{
			for (size_t i = 0; i < keyA->getLength(); ++i)
			{
				data.push_back(0x00);
			}
		}

		data.push_back(static_cast<unsigned char>(keyB->isEmpty() ? 0 : 1));

		if (!keyB->isEmpty())
		{
			unsigned char* keydata = keyB->getData();
			data.insert(data.end(), keydata, keydata + keyB->getLength());
		}
		else
		{
			for (size_t i = 0; i < keyB->getLength(); ++i)
			{
				data.push_back(0x00);
			}
		}

		unsigned char sabbuf[3];
		memset(sabbuf, 0x00, sizeof(sabbuf));
		sab.toArray(sabbuf, sizeof(sabbuf));
		data.insert(data.end(), sabbuf, sabbuf + sizeof(sabbuf));
		data.push_back(useMAD ? 0x01 : 0x00);

		data.push_back(static_cast<unsigned char>(madKeyA->isEmpty() ? 0 : 1));

		if (!madKeyA->isEmpty())
		{
			unsigned char* keydata = madKeyA->getData();
			data.insert(data.end(), keydata, keydata + madKeyA->getLength());
		}
		else
		{
			for (size_t i = 0; i < madKeyA->getLength(); ++i)
			{
				data.push_back(0x00);
			}
		}

		data.push_back(static_cast<unsigned char>(madKeyB->isEmpty() ? 0 : 1));

		if (!madKeyB->isEmpty())
		{
			unsigned char* keydata = madKeyB->getData();
			data.insert(data.end(), keydata, keydata + madKeyB->getLength());
		}
		else
		{
			for (size_t i = 0; i < madKeyB->getLength(); ++i)
			{
				data.push_back(0x00);
			}
		}

		return data;
	}

	void MifareAccessInfo::setLinearData(const std::vector<unsigned char>& data, size_t offset)
	{
		bool hasKeyA = (data[offset++] == 1);

		if (hasKeyA)
		{
			keyA->setData(data, offset);
		}
		else
		{
			keyA->setData(std::vector<unsigned char>());
		}
		offset += keyA->getLength();

		bool hasKeyB = (data[offset++] == 1);

		if (hasKeyB)
		{
			keyB->setData(data, offset);
		}
		else
		{
			keyB->setData(std::vector<unsigned char>());
		}
		offset += keyB->getLength();

		sab.fromArray(&data[offset], 3);
		offset += 3;
		useMAD = (data[offset++] == 0x01);

		bool hasMadKeyA = (data[offset++] == 1);

		if (hasMadKeyA)
		{
			madKeyA->setData(data, offset);
		}
		else
		{
			madKeyA->setData(std::vector<unsigned char>());
		}
		offset += madKeyA->getLength();

		bool hasMadKeyB = (data[offset++] == 1);

		if (hasMadKeyB)
		{
			madKeyB->setData(data, offset);
		}
		else
		{
			madKeyB->setData(std::vector<unsigned char>());
		}
		offset += madKeyB->getLength();
	}

	size_t MifareAccessInfo::getDataSize()
	{
		return ((MIFARE_KEY_SIZE * 2) + sizeof(sab) + sizeof(useMAD) + (MIFARE_KEY_SIZE * 2) + 4);
	}

	void MifareAccessInfo::serialize(boost::property_tree::ptree& parentNode)
	{
		unsigned char buf[3];
		boost::property_tree::ptree node;

		sab.toArray(buf, sizeof(buf));
		std::ostringstream oss;		
		oss << std::setfill('0');
		for (size_t i = 0; i < sizeof(buf); ++i)
		{
			oss << std::setw(2) << std::hex << static_cast<unsigned int>(buf[i]);
			if (i < sizeof(buf) - 1)
			{
				oss << " ";
			}
		}

		boost::property_tree::ptree ka;
		keyA->serialize(ka);
		node.add_child("KeyA", ka);
		boost::property_tree::ptree kb;
		keyB->serialize(kb);
		node.add_child("KeyB", kb);
		node.put("SectorAccessBits", oss.str());

		boost::property_tree::ptree madnode;
		madnode.put("Use", useMAD);
		boost::property_tree::ptree madka;
		madKeyA->serialize(madka);
		madnode.add_child("MADKeyA", madka);
		boost::property_tree::ptree madkb;
		madKeyB->serialize(madkb);
		madnode.add_child("MADKeyB", madkb);
		node.add_child("MAD", madnode);

		parentNode.add_child(getDefaultXmlNodeName(), node);
	}

	void MifareAccessInfo::unSerialize(boost::property_tree::ptree& node)
	{
		boost::dynamic_pointer_cast<XmlSerializable>(keyA)->unSerialize(node.get_child("KeyA"), "");
		boost::dynamic_pointer_cast<XmlSerializable>(keyB)->unSerialize(node.get_child("KeyB"), "");

		string sabstr = node.get_child("SectorAccessBits").get_value<std::string>();
		unsigned char buf[3];
		memset(buf, 0x00, sizeof(buf));
		std::istringstream iss(sabstr);
		for (size_t i = 0; i < sizeof(buf); ++i)
		{
			unsigned int tmp;
			if (!iss.good())
			{
				return;
			}
			iss >> std::hex >> tmp;

			buf[i] = static_cast<unsigned char>(tmp);
		}
		sab.fromArray(buf, sizeof(buf));

		boost::property_tree::ptree modnode = node.get_child("MAD");
		if (!modnode.empty())
		{
			useMAD = modnode.get_child("Use").get_value<bool>();
			boost::dynamic_pointer_cast<XmlSerializable>(madKeyA)->unSerialize(modnode.get_child("MADKeyA"), "");
			boost::dynamic_pointer_cast<XmlSerializable>(madKeyB)->unSerialize(modnode.get_child("MADKeyB"), "");
		}
	}

	std::string MifareAccessInfo::getDefaultXmlNodeName() const
	{
		return "MifareAccessInfo";
	}

	bool MifareAccessInfo::operator==(const AccessInfo& ai) const
	{
		if (!AccessInfo::operator==(ai))
			return false;

		const MifareAccessInfo* mAi = dynamic_cast<const MifareAccessInfo*>(&ai);

		return (keyA == mAi->keyA
			&& keyB == mAi->keyB
			&& madKeyA == mAi->madKeyA
			&& madKeyB == mAi->madKeyB
			&& useMAD == mAi->useMAD
			);
	}
}

