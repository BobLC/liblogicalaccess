/**
 * \file desfireev1chip.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief DESFire EV1 chip.
 */

#ifndef LOGICALACCESS_SAMAV2CHIP_HPP
#define LOGICALACCESS_SAMAV2CHIP_HPP

#include "logicalaccess/cards/chip.hpp"
#include "samcommands.hpp"
#include "samchip.hpp"


namespace logicalaccess
{	
	#define CHIP_SAMAV2	"SAM_AV2"

	/**
	 * \brief The SAM chip class.
	 */
	class LIBLOGICALACCESS_API SAMAV2Chip : public SAMChip
	{
		public:

			/**
			 * \brief Constructor.
			 */
			SAMAV2Chip();

			SAMAV2Chip(std::string ct);

			/**
			 * \brief Destructor.
			 */
			~SAMAV2Chip();


			boost::shared_ptr<SAMCommands> getSAMAV1Commands() { return boost::dynamic_pointer_cast<SAMCommands>(getCommands()); };
		protected:
	};
}

#endif /* LOGICALACCESS_DESFIREEV1CHIP_HPP */

