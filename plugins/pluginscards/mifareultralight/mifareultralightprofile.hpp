/**
 * \file mifareultralightprofile.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Mifare Ultralight card profiles.
 */

#ifndef LOGICALACCESS_MIFAREULTRALIGHT_HPP
#define LOGICALACCESS_MIFAREULTRALIGHT_HPP

#include "logicalaccess/cards/accessinfo.hpp"
#include "mifareultralightlocation.hpp"
#include "logicalaccess/cards/profile.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace logicalaccess
{
	/**
	 * \brief The Mifare Ultralight base profile class.
	 */
	class LIBLOGICALACCESS_API MifareUltralightProfile : public Profile
	{
		public:			

			/**
			 * \brief Constructor.
			 */
			MifareUltralightProfile();

			/**
			 * \brief Destructor.
			 */
			virtual ~MifareUltralightProfile();		

			/**
			 * \brief Set default keys for the mifare card in memory.
			 */
			virtual void setDefaultKeys();

			/**
			 * \brief Set default keys for the type card in memory at a specific location.
			 */
			virtual void setDefaultKeysAt(boost::shared_ptr<Location> location);

			/**
			 * \brief Clear all keys in memory.
			 */
			virtual void clearKeys();

			/**
			 * \brief Set key at a specific location.
			 * \param location The key's location.
			 * \param AccessInfo The key's informations.
			 */
			virtual void setKeyAt(boost::shared_ptr<Location> location, boost::shared_ptr<AccessInfo> AccessInfo);

			/**
			 * \brief Set default keys for a sector.
			 * \param index The sector index.
			 */
			void setDefaultKeysAt(size_t index);

			/**
			 * \brief Create default access informations.
			 * \return Default access informations. Always null.
			 */
			virtual boost::shared_ptr<AccessInfo> createAccessInfo() const;

			/**
			 * \brief Create default location.
			 * \return Default location.
			 */
			virtual boost::shared_ptr<Location> createLocation() const;

			/**
			 * \brief Get the supported format list.
			 * \return The format list.
			 */
			virtual FormatList getSupportedFormatList();

		protected:

	};
}

#endif
