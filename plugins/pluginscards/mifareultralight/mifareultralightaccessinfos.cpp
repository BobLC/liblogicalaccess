/**
 * \file mifareultralightaccessinfo.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Access informations for Mifare Ultralight card.
 */

#include "mifareultralightaccessinfo.hpp"
#include "mifareultralightchip.hpp"

namespace logicalaccess
{
	MifareUltralightAccessInfo::MifareUltralightAccessInfo()
	{
		lockPage = false;
	}

	MifareUltralightAccessInfo::~MifareUltralightAccessInfo()
	{		
	}

	std::string MifareUltralightAccessInfo::getCardType() const
	{
		return CHIP_MIFAREULTRALIGHT;
	}

	void MifareUltralightAccessInfo::generateInfos()
	{		
		
	}

	void MifareUltralightAccessInfo::generateInfos(const std::string& /*csn*/)
	{
		
	}

	std::vector<unsigned char> MifareUltralightAccessInfo::getLinearData() const
	{
		std::vector<unsigned char> data;
		
		data.push_back(static_cast<unsigned char>(lockPage ? 1 : 0));

		return data;
	}

	void MifareUltralightAccessInfo::setLinearData(const std::vector<unsigned char>& data, size_t offset)
	{
		lockPage = (data[offset++] == 1);
	}

	size_t MifareUltralightAccessInfo::getDataSize()
	{
		return (1);
	}

	void MifareUltralightAccessInfo::serialize(boost::property_tree::ptree& parentNode)
	{
		boost::property_tree::ptree node;
		node.put("LockPage", lockPage);
		parentNode.add_child(getDefaultXmlNodeName(), node);
	}

	void MifareUltralightAccessInfo::unSerialize(boost::property_tree::ptree& node)
	{
		lockPage = node.get_child("LockPage").get_value<bool>();
	}

	std::string MifareUltralightAccessInfo::getDefaultXmlNodeName() const
	{
		return "MifareUltralightAccessInfo";
	}

	bool MifareUltralightAccessInfo::operator==(const AccessInfo& ai) const
	{
		if (!AccessInfo::operator==(ai))
			return false;

		const MifareUltralightAccessInfo* mAi = dynamic_cast<const MifareUltralightAccessInfo*>(&ai);

		return (lockPage == mAi->lockPage
			);
	}
}

