#ifndef PACKET_COMPONENT_H
#define PACKET_COMPONENT_H

//INCLUDE DEPENDENCIES
#include <SFML\Network.hpp>

//DEFINE GLOBAL ENUMS

//ENUM USED AS A PACKET HEADER TO TELL RECEIVING PROGRAM HOW TO HANDLE THE PACKET
enum Packet_Type {
	packet_type_none,
	packet_type_player_name,
	packet_type_player_list,
	packet_type_player_id_assign,
	packet_type_chat_line_incomplete,
	packet_type_chat_line_complete,
	packet_type_confirm,
	packet_type_deny,
	packet_type_connection_test,
	packet_type_request_name,
	packet_disconnect,
	Packet_Type_First = packet_type_none,
	Packet_Type_Last = packet_disconnect
};

//INHERIT FROM THIS CLASS IS THE CLASS CAN BE WRITTEN/READ AS A PACKET __OR__ A COMPONENT OF SUCH A CLASS
class Packet_Component {
protected:
	Packet_Type _packet_type;

	//SPECIAL FUNCTIONS THAT ADD A PACKET TYPE TO THE BEGINNING OF A PACKET WHEN 'CREATED' (NOT WRITTEN) BY THE OUTTERMOST OBJECT BEING ENCAPSULATED IN A PACKET
	void packet_type_write(sf::Packet &packet);
	void packet_type_write_none(sf::Packet &packet);
	//METHODS FOR READING/WRITING UINTS INTO/FROM PACKETS
	void packet_write_uint(sf::Packet &packet, const unsigned int &num);
	bool packet_read_uint(sf::Packet &packet, unsigned int &num);
public:
	void change_packet_type(Packet_Type to);
	//MAKE A PACKET
	void packet_create(sf::Packet &packet);
	//WRITE COMPONENTS INTO THE PACKET
	virtual void packet_write(sf::Packet &packet) = 0;
	//READ COMPONENTS FROM PACKET
	virtual bool packet_read(sf::Packet &packet) = 0;

	//TEMPLATES HAVE TO BE DEFINED IN THE HEADER APPARENTLY
	template <typename T>
	void vec_write_component(sf::Packet &packet, std::vector <T> &read_from) {
		packet_write_uint(packet, static_cast<unsigned int>(read_from.size()));
		for (auto i = read_from.begin(); i != read_from.end(); i++) {
			(*i).packet_write(packet);
		}
	}

	template <typename T>
	bool vec_read_component(sf::Packet &packet, std::vector <T> &write_to) {
		unsigned int vec_size = 0;
		if (! packet_read_uint(packet, vec_size)) {
			//RETURN FALSE IF THE PACKET SIZE COULD NOT BE READ
			return false;
		}

		//RESERVE FIRST TO AVOID INVALIDATING EXISTING POINTERS TO THESE ELEMENTS ON RESIZE
		write_to.reserve(vec_size);
		//RESIZE VECTOR TO ACCOMODATE POTENTIAL NEW ELEMENTS
		write_to.resize(vec_size);

		for (auto i = write_to.begin(); i != write_to.end(); i++) {
			if (! (*i).packet_read(packet)) {
				return false;
			}
		}

		return true;
	}

	Packet_Component();
	Packet_Component(Packet_Type packet_type);
	virtual ~Packet_Component() {}
};


////ADDITIONAL CLASS EXAMPLE
class Player_Name : public Packet_Component {
protected:
	std::string _name;
public:
	std::string name();
	void name(std::string to);
	virtual void packet_write(sf::Packet &packet);
	virtual bool packet_read(sf::Packet &packet);
	Player_Name();
	Player_Name(std::string name);
};

//ADDITIONAL CLASS EXAMPLE
class Player_ID : public Packet_Component {
protected:
	unsigned int _id;
public:
	unsigned int id();
	void id(unsigned int to);
	virtual void packet_write(sf::Packet &packet);
	virtual bool packet_read(sf::Packet &packet);
	Player_ID();
	Player_ID(unsigned int id);
};
#endif 

