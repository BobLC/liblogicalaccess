/**
 * \file libnfcreaderunit.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC Reader unit.
 */

#ifndef LOGICALACCESS_LIBNFCREADERUNIT_HPP
#define LOGICALACCESS_LIBNFCREADERUNIT_HPP

#include "logicalaccess/readerproviders/readerunit.hpp"
#include "libnfcreaderunitconfiguration.hpp"
#include "logicalaccess/cards/readermemorykeystorage.hpp"

namespace logicalaccess
{
	class Chip;
	class LibNFCReaderCardAdapter;
	class LibNFCReaderProvider;


	/**
	 * \brief The LibNFC reader unit class.
	 */
	class LIBLOGICALACCESS_API LibNFCReaderUnit : public ReaderUnit
	{
		public:			

			/**
			 * \brief Constructor.
			 */
			LibNFCReaderUnit(const std::string& name);

			/**
			 * \brief Destructor.
			 */
			virtual ~LibNFCReaderUnit();

			/**
			 * \brief Get the reader unit name.
			 * \return The reader unit name.
			 */
			virtual std::string getName() const;

			/**
			 * \brief Set the reader unit name.
			 * \param name The reader unit name.
			 */
			void setName(const std::string& name);

			/**
			 * \brief Get the connected reader unit name.
			 * \return The connected reader unit name.
			 */
			virtual std::string getConnectedName();

			/**
			 * \brief Set the card type.
			 * \param cardType The card type.
			 */
			virtual void setCardType(std::string cardType) { d_card_type = cardType; };			

			/**
			 * \brief Wait for a card insertion.
			 * \param maxwait The maximum time to wait for, in milliseconds. If maxwait is zero, then the call never times out.
			 * \return True if a card was inserted, false otherwise. If a card was inserted, the name of the reader on which the insertion was detected is accessible with getReader().
			 * \warning If the card is already connected, then the method always fail.
			 */
			virtual bool waitInsertion(unsigned int maxwait);

			/**
			 * \brief Wait for a card removal.
			 * \param maxwait The maximum time to wait for, in milliseconds. If maxwait is zero, then the call never times out.
			 * \return True if a card was removed, false otherwise. If a card was removed, the name of the reader on which the removal was detected is accessible with getReader().
			 */
			virtual bool waitRemoval(unsigned int maxwait);

			/**
			 * \brief Connect to the card.
			 * \return True if the card was connected without error, false otherwise.
			 *
			 * If the card handle was already connected, connect() first call disconnect(). If you intend to do a reconnection, call reconnect() instead.
			 */
			virtual bool connect();

			/**
			 * \brief Disconnect from the card.
			 */
			virtual void disconnect();

			/**
			 * \brief Check if the handle is associated to a card (aka. "connected").
			 * \return True if the handle is associated to a card, false otherwise.
			 */
			virtual bool isConnected();

			/**
			 * \brief Connect to the reader. Implicit connection on first command sent.
			 * \return True if the connection successed.
			 */
			virtual bool connectToReader();

			/**
			 * \brief Disconnect from reader.
			 */
			virtual void disconnectFromReader();

			/**
			 * \brief Get the low-level SCARDHANDLE.
			 * \return The SCARDHANDLE.
			 */
			const SCARDHANDLE& getHandle() const;

			/**
			 * \brief Create the chip object from card type.
			 * \param type The card type.
			 * \return The chip.
			 */
			virtual boost::shared_ptr<Chip> createChip(std::string type);

			/**
			 * \brief Get the first and/or most accurate chip found.
			 * \return The single chip.
			 */
			virtual boost::shared_ptr<Chip> getSingleChip();

			/**
			 * \brief Get chip available in the RFID rang.
			 * \return The chip list.
			 */
			virtual std::vector<boost::shared_ptr<Chip> > getChipList();

			/**
			 * \brief Get the card serial number.
			 * \return The card serial number.
			 *
			 * If the card handle is not connected to the card, the call fails.
			 */
			virtual std::vector<unsigned char> getCardSerialNumber();			

			/**
			 * \brief Get a string hexadecimal representation of the reader serial number
			 * \return The reader serial number or an empty string on error.
			 */
			virtual std::string getReaderSerialNumber();

			/**
			 * \brief Get the card ATR.
			 * \param atr The array that will contains the ATR data.
			 * \param atrLength The ATR array length.
			 * \return The ATR length.
			 */
			unsigned int getATR(void* atr, size_t atrLength);

			/**
			 * \brief Get the default LibNFC reader/card adapter.
			 * \return The default LibNFC reader/card adapter.
			 */
			virtual boost::shared_ptr<LibNFCReaderCardAdapter> getDefaultLibNFCReaderCardAdapter();

			/**
			 * \brief Get the LibNFC reader/card adapter for a card type.
			 * \param type The card type.
			 * \return The LibNFC reader/card adapter.
			 */
			virtual boost::shared_ptr<ReaderCardAdapter> getReaderCardAdapter(std::string type);

			/**
			 * \brief Serialize the current object to XML.
			 * \param parentNode The parent node.
			 */
			virtual void serialize(boost::property_tree::ptree& parentNode);

			/**
			 * \brief UnSerialize a XML node to the current object.
			 * \param node The XML node.
			 */
			virtual void unSerialize(boost::property_tree::ptree& node);

			/**
			 * \brief Get the reader unit configuration.
			 * \return The reader unit configuration.
			 */
			virtual boost::shared_ptr<ReaderUnitConfiguration> getConfiguration();

			/**
			 * \brief Set the reader unit configuration.
			 * \param config The reader unit configuration.
			 */
			virtual void setConfiguration(boost::shared_ptr<ReaderUnitConfiguration> config);

			/**
			 * \brief Change a key in reader memory.
			 * \param keystorage The key storage information.
			 * \param key The key value.
			 */
			virtual void changeReaderKey(boost::shared_ptr<ReaderMemoryKeyStorage> keystorage, const std::vector<unsigned char>& key);

			/**
			 * \brief Get the LibNFC reader unit configuration.
			 * \return The LibNFC reader unit configuration.
			 */
			boost::shared_ptr<LibNFCReaderUnitConfiguration> getLibNFCConfiguration() { return boost::dynamic_pointer_cast<LibNFCReaderUnitConfiguration>(getConfiguration()); };

			boost::shared_ptr<LibNFCReaderProvider> getLibNFCReaderProvider() const;

			static boost::shared_ptr<LibNFCReaderUnit> createLibNFCReaderUnit(std::string& readerName);

			/**
			 * \brief Make the reader unit as a proxy to another. Use when listening on all LibNFC reader unit.
			 * \param readerUnit The referenced reader unit.
			 * \param readerUnitConfig The referenced reader unit configuration.
			 */
			void makeProxy(boost::shared_ptr<LibNFCReaderUnit> readerUnit, boost::shared_ptr<LibNFCReaderUnitConfiguration> readerUnitConfig);

		protected:

			/**
			 * \brief The reader unit name.
			 */
			std::string d_name;

			/**
			 * \brief The reader unit connected name.
			 */
			std::string d_connectedName;

			/**
			 * \brief Set the first and/or most accurate chip found.
			 * \param chip The single chip.
			 */
			void setSingleChip(boost::shared_ptr<Chip> chip);

			/**
			 * \brief The handle.
			 */
			SCARDHANDLE d_sch;

			/**
			 * \brief The current card ATR.
			 */
			unsigned char d_atr[64];

			/**
			 * \brief The ATR length.
			 */
			size_t d_atrLength;		
	};
}

#endif
