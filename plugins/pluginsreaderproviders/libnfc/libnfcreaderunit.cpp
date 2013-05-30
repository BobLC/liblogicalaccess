/**
 * \file libnfcreaderunit.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC reader unit.
 */

#include "libnfcreaderunit.hpp"


#include <iostream>
#include <iomanip>
#include <sstream>

#include "libnfcreaderprovider.hpp"
#include "logicalaccess/services/accesscontrol/cardsformatcomposite.hpp"
#include "logicalaccess/cards/chip.hpp"
#include "logicalaccess/dynlibrary/librarymanager.hpp"
#include "logicalaccess/dynlibrary/idynlibrary.hpp"

#include "desfireev1cardprovider.hpp"
#include "commands/desfireev1iso7816commands.hpp"
#include "mifarecardprovider.hpp"
#include "commands/mifarelibnfccommands.hpp"
#include "commands/iso7816iso7816commands.hpp"
#include "commands/twiciso7816commands.hpp"
#include "mifareultralightcardprovider.hpp"
#include "commands/mifareultralightlibnfccommands.hpp"
#include "mifareultralightccardprovider.hpp"
#include "commands/mifareultralightclibnfccommands.hpp"
#include "readercardadapters/libnfcreadercardadapter.hpp"

#include "desfirechip.hpp"
#include <boost/filesystem.hpp>

#ifdef UNIX
#include <sys/time.h>
#endif

namespace logicalaccess
{
	LibNFCReaderUnit::LibNFCReaderUnit(const std::string& name)
		: ReaderUnit()
	{
		d_name = name;
		d_connectedName = d_name;
		d_sch = 0;
		d_atrLength = 0;


		d_card_type = "UNKNOWN";

		try
		{
			boost::property_tree::ptree pt;
			read_xml((boost::filesystem::current_path().string() + "/LibNFCReaderUnit.config"), pt);
			d_card_type = pt.get("config.cardType", "UNKNOWN");
		}
		catch (...) { }

		d_proxyReaderUnit.reset();
		d_readerUnitConfig.reset(new LibNFCReaderUnitConfiguration());
		d_defaultReaderCardAdapter.reset(new LibNFCReaderCardAdapter());
	}

	LibNFCReaderUnit::~LibNFCReaderUnit()
	{
		disconnect();
	}

	std::string LibNFCReaderUnit::getName() const
	{
		return d_name;
	}

	void LibNFCReaderUnit::setName(const std::string& name)
	{
		d_name = name;
	}

	const SCARDHANDLE& PCSCReaderUnit::getHandle() const
	{
		if (d_proxyReaderUnit)
		{
			return d_proxyReaderUnit->getHandle();
		}

		return d_sch;
	}

	void LibNFCReaderUnit::serialize(boost::property_tree::ptree& parentNode)
	{
		boost::property_tree::ptree node;

		node.put("<xmlattr>.type", getReaderProvider()->getRPType());
		node.put("Name", d_name);
		getConfiguration()->serialize(node);

		parentNode.add_child(getDefaultXmlNodeName(), node);
	}

	void LibNFCReaderUnit::unSerialize(boost::property_tree::ptree& node)
	{
		d_name = node.get_child("Name").get_value<std::string>();
		boost::shared_ptr<LibNFCReaderUnitConfiguration> pcscRUC = getLibNFCConfiguration();	
		getConfiguration()->unSerialize(node.get_child(getConfiguration()->getDefaultXmlNodeName()));
	}

	boost::shared_ptr<LibNFCReaderUnit> LibNFCReaderUnit::createLibNFCReaderUnit(std::string& readerName)
	{
		boost::shared_ptr<LibNFCReaderUnit> unit;

		if (!unit)
		{
			unit.reset(new LibNFCReaderUnit(readerName));
		}

		return unit;
	}

	bool LibNFCReaderUnit::isConnected()
	{
		return (d_sch != 0);
	}

	bool LibNFCReaderUnit::waitInsertion(unsigned int maxwait)
	{
		if (isConnected())
		{
			THROW_EXCEPTION_WITH_LOG(CardException, EXCEPTION_MSG_CONNECTED);
		}

		int readers_count = static_cast<int>(getReaderProvider()->getReaderList().size());

		string reader = getName();
		string connectedReader = "";
		std::string cardType = "UNKNOWN";

		if (reader != "")
		{
			readers_count = 1;
		}

		if (readers_count == 0)
		{
			THROW_EXCEPTION_WITH_LOG(CardException, EXCEPTION_MSG_NOREADER);
		}

		return (connectedReader != "");
	}

	bool LibNFCReaderUnit::waitRemoval(unsigned int maxwait)
	{
		if (isConnected())
		{
			THROW_EXCEPTION_WITH_LOG(CardException, EXCEPTION_MSG_CONNECTED);
		}

		string reader = getConnectedName();

		int readers_count = static_cast<int>(getReaderProvider()->getReaderList().size());

		if (reader != "")
		{
			readers_count = 1;
		}

		if (readers_count == 0)
		{
			THROW_EXCEPTION_WITH_LOG(CardException, EXCEPTION_MSG_NOREADER);
		}

		return (!reader.empty());
	}

	bool LibNFCReaderUnit::connect()
	{
		return connect(getPCSCConfiguration()->getShareMode());
	}

	void LibNFCReaderUnit::disconnect()
	{
		if (isConnected())
		{			
			SCardDisconnect(d_sch, SCARD_LEAVE_CARD);
		}

		d_sch = 0;
		d_ap = 0;
	}

	bool LibNFCReaderUnit::connectToReader()
	{
		return true;
	}

	void LibNFCReaderUnit::disconnectFromReader()
	{
	}

	std::vector<unsigned char> LibNFCReaderUnit::getCardSerialNumber()
	{
		std::vector<unsigned char> csn;
		unsigned char ucReceivedData[255];
		size_t ulNoOfDataReceived = sizeof(ucReceivedData);	// max csn len 64 (+ SW1 and SW2)

#ifdef _LICENSE_SYSTEM
		if (d_license->hasReadDataAccess())
#endif
		{
			memset(ucReceivedData, 0x00, sizeof(ucReceivedData));
			getDefaultPCSCReaderCardAdapter()->sendAPDUCommand(0xFF, 0xCA, 0x00, 0x00, 0x00, ucReceivedData, &ulNoOfDataReceived);

			csn = std::vector<unsigned char>(ucReceivedData, ucReceivedData + (ulNoOfDataReceived - 2));
		}

		return csn;
	}

	size_t LibNFCReaderUnit::getATR(void* atr, size_t atrLength)
	{
		if (atr != NULL && atrLength >= d_atrLength)
		{
			memcpy(atr, d_atr, d_atrLength);
		}

		return d_atrLength;
	}

	boost::shared_ptr<ReaderCardAdapter> LibNFCReaderUnit::getReaderCardAdapter(std::string type)
	{
		return getDefaultReaderCardAdapter();
	}

	boost::shared_ptr<Chip> LibNFCReaderUnit::createChip(std::string type)
	{
		std::string getterName;

		if (d_proxyReaderUnit)
		{
			return d_proxyReaderUnit->createChip(type);
		}

		boost::shared_ptr<Chip> chip = ReaderUnit::createChip(type);
		if (chip)
		{
			boost::shared_ptr<ReaderCardAdapter> rca = getReaderCardAdapter(type);
			boost::shared_ptr<CardProvider> cp;
			boost::shared_ptr<Commands> commands;

			if (type ==  "Mifare1K" || type == " Mifare4K" || type == " Mifare")
			{
				cp.reset(new MifareCardProvider());
				commands.reset(new MifareLibNFCCommands());
			}
			else if (type == "DESFireEV1")
			{
				cp.reset(new DESFireEV1CardProvider());
				commands.reset(new DESFireEV1ISO7816Commands());
				boost::dynamic_pointer_cast<DESFireEV1ISO7816Commands>(commands)->getCrypto().setCryptoContext(boost::dynamic_pointer_cast<DESFireProfile>(chip->getProfile()), chip->getChipIdentifier());
			}
			else if (type == "DESFire")
			{
				cp.reset(new DESFireCardProvider());
				commands.reset(new DESFireISO7816Commands());
				boost::dynamic_pointer_cast<DESFireISO7816Commands>(commands)->getCrypto().setCryptoContext(boost::dynamic_pointer_cast<DESFireProfile>(chip->getProfile()), chip->getChipIdentifier());
			}
			else if (type == "MifareUltralight")
			{
				cp.reset(new MifareUltralightCardProvider());
				commands.reset(new MifareUltralightLibNFCCommands());
			}
			else if (type == "MifareUltralightC")
			{
				cp.reset(new MifareUltralightCCardProvider());
				commands.reset(new MifareUltralightCLibNFCCommands());
			}

			if (!cp)
			{
				cp = LibraryManager::getInstance()->getCardProvider(type);
			}

			rca->setReaderUnit(shared_from_this());
			if (commands)
			{
				commands->setReaderCardAdapter(rca);
			}
			if (cp)
			{
				cp->setCommands(commands);
			}
			chip->setCardProvider(cp);
		}
		return chip;
	}

	boost::shared_ptr<Chip> LibNFCReaderUnit::getSingleChip()
	{
		boost::shared_ptr<Chip> chip = d_insertedChip;
		return chip;
	}

	std::vector<boost::shared_ptr<Chip> > LibNFCReaderUnit::getChipList()
	{
		std::vector<boost::shared_ptr<Chip> > chipList;
		boost::shared_ptr<Chip> singleChip = getSingleChip();
		if (singleChip)
		{
			chipList.push_back(singleChip);
		}
		return chipList;
	}

	boost::shared_ptr<LibNFCReaderCardAdapter> LibNFCReaderUnit::getDefaultLibNFCReaderCardAdapter()
	{
		boost::shared_ptr<ReaderCardAdapter> adapter = getDefaultReaderCardAdapter();
		if (d_proxyReaderUnit)
		{
			adapter->setReaderUnit(d_proxyReaderUnit);
		}
		return boost::dynamic_pointer_cast<LibNFCReaderCardAdapter>(adapter);
	}

	string LibNFCReaderUnit::getReaderSerialNumber()
	{
		if (d_proxyReaderUnit)
		{
			return d_proxyReaderUnit->getReaderSerialNumber();
		}
		return "";
	}

	boost::shared_ptr<LibNFCReaderProvider> LibNFCReaderUnit::getLibNFCReaderProvider() const
	{
		return boost::dynamic_pointer_cast<LibNFCReaderProvider>(getReaderProvider());
	}

	void LibNFCReaderUnit::setSingleChip(boost::shared_ptr<Chip> chip)
	{
		d_insertedChip = chip;
	}

	boost::shared_ptr<ReaderUnitConfiguration> LibNFCReaderUnit::getConfiguration()
	{
		return d_readerUnitConfig;
	}

	void LibNFCReaderUnit::setConfiguration(boost::shared_ptr<ReaderUnitConfiguration> config)
	{
		d_readerUnitConfig = config;
	}
}
