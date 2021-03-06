/**
 * \file desfirechip.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief DESFire chip.
 */

#include "desfirechip.hpp"
#include "desfireprofile.hpp"
#include "logicalaccess/services/accesscontrol/accesscontrolcardservice.hpp"
#include "desfirestoragecardservice.hpp"

#include <cstring>

namespace logicalaccess
{
	DESFireChip::DESFireChip(std::string ct) : 
		Chip(ct)
	{
		
	}

	DESFireChip::DESFireChip() : 
		Chip(CHIP_DESFIRE)
	{
		d_profile.reset(new DESFireProfile());
	}

	DESFireChip::~DESFireChip()
	{

	}

	boost::shared_ptr<LocationNode> DESFireChip::getRootLocationNode()
	{
		boost::shared_ptr<LocationNode> rootNode;
		rootNode.reset(new LocationNode());
		char tmpName[255];

		rootNode->setName("Mifare DESFire");
		rootNode->setHasProperties(true);

		boost::shared_ptr<DESFireLocation> rootLocation = boost::dynamic_pointer_cast<DESFireLocation>(getProfile()->createLocation());
		rootLocation->aid = (unsigned int)-1;
		rootLocation->file = (unsigned int)-1;
		rootNode->setLocation(rootLocation);

		if (getCommands())
		{
			getDESFireCommands()->selectApplication(0);

			// Try authentication.
			try
			{
				getDESFireCommands()->authenticate(0);
			}
			catch(CardException&)
			{

			}

			std::vector<int> aids = getDESFireCommands()->getApplicationIDs();

			for (std::vector<int>::const_iterator aid = aids.begin(); aid != aids.end(); aid++)
			{
				boost::shared_ptr<LocationNode> aidNode;
				aidNode.reset(new LocationNode());
				sprintf(tmpName, "Application ID %u", *aid);
				aidNode->setName(tmpName);

				boost::shared_ptr<DESFireLocation> aidLocation = boost::dynamic_pointer_cast<DESFireLocation>(getProfile()->createLocation());
				aidLocation->aid = *aid;
				aidLocation->file = static_cast<unsigned int>(-1);
				aidNode->setLocation(aidLocation);

				if (getDESFireCommands()->selectApplication(*aid))
				{
					// Try authentication.
					try
					{
						getDESFireCommands()->authenticate(0);
					}
					catch(CardException&)
					{

					}

					try
					{
						std::vector<int> files = getDESFireCommands()->getFileIDs();
					
						for (std::vector<int>::const_iterator file = files.begin(); file != files.end(); ++file)
						{
							boost::shared_ptr<LocationNode> fileNode;
							fileNode.reset(new LocationNode());
							sprintf(tmpName, "File %d", *file);
							fileNode->setName(tmpName);

							boost::shared_ptr<DESFireLocation> location = getApplicationLocation();
							location->aid = *aid;
							location->file = *file;
							location->byte = 0;							

							try
							{
								DESFireCommands::FileSetting settings;
								if (getDESFireCommands()->getFileSettings(*file, settings))
								{
									location->securityLevel = (EncryptionMode)settings.comSett;
									switch (settings.fileType)
									{
									case 0:
										{
											size_t fileSize = 0;
											memcpy(&fileSize, settings.type.dataFile.fileSize, sizeof(settings.type.dataFile.fileSize));
											fileNode->setLength(fileSize);
										}
										break;

									case 1:
										{
											//TODO: Write something here ?
										}
										break;

									case 2:
										{
											size_t recordSize = 0;
											memcpy(&recordSize, settings.type.recordFile.recordSize, sizeof(settings.type.recordFile.recordSize));
											fileNode->setLength(recordSize);
										}
										break;
									}
								}
								else
								{
									location->securityLevel = CM_ENCRYPT;
								}
							}
							catch(std::exception&)
							{
								fileNode->setLength(0);
							}
															
							fileNode->setNeedAuthentication(true);
							fileNode->setHasProperties(true);
							fileNode->setLocation(location);
							fileNode->setParent(aidNode);
							aidNode->getChildrens().push_back(fileNode);
						}
					}
					catch(std::exception&)
					{
					}
				}

				aidNode->setHasProperties(true);
				aidNode->setParent(rootNode);
				rootNode->getChildrens().push_back(aidNode);
			}
		}

		return rootNode;
	}

	boost::shared_ptr<DESFireLocation> DESFireChip::getApplicationLocation()
	{
		boost::shared_ptr<DESFireLocation> location(new DESFireLocation());		

		return location;
	}

	boost::shared_ptr<CardService> DESFireChip::getService(CardServiceType serviceType)
	{
		boost::shared_ptr<CardService> service;

		switch (serviceType)
		{
		case CST_ACCESS_CONTROL:
			{
				service.reset(new AccessControlCardService(shared_from_this()));
			}
			break;
		case CST_STORAGE:
			{
				service.reset(new DESFireStorageCardService(shared_from_this()));
			}
			break;
		case CST_NFC_TAG:
		  break;
		}

		if (!service)
		{
			service = Chip::getService(serviceType);
		}

		return service;
	}
}
