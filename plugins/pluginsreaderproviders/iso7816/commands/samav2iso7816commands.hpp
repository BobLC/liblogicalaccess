/**
 * \file desfireiso7816commands.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief DESFire commands.
 */

#ifndef LOGICALACCESS_SAMAV2ISO7816CARDPROVIDER_HPP
#define LOGICALACCESS_SAMAV2ISO7816CARDPROVIDER_HPP

#include "samcommands.hpp"
#include "samav1iso7816commands.hpp"
#include "../readercardadapters/iso7816readercardadapter.hpp"
#include "../iso7816readerunitconfiguration.hpp"
#include "samcrypto.hpp"
#include "samkeyentry.hpp"
#include "samcrypto.hpp"
#include <string>
#include <vector>
#include <iostream>


namespace logicalaccess
{
	/**
	 * \brief The DESFire base commands class.
	 */
	class LIBLOGICALACCESS_API SAMAV2ISO7816Commands : public SAMAV1ISO7816Commands
	{
		public:

			/**
			 * \brief Constructor.
			 */
			SAMAV2ISO7816Commands();

			/**
			 * \brief Destructor.
			 */
			virtual ~SAMAV2ISO7816Commands();			

		protected:
	};
}

#endif /* LOGICALACCESS_DESFIREISO7816COMMANDS_HPP */

