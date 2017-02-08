#include "packet_component.h"

Packet_Component::Packet_Component() :
	_packet_type(packet_type_none)
{

}
Packet_Component::Packet_Component(Packet_Type packet_type) :
	_packet_type(packet_type) {

}
void Packet_Component::packet_type_write(sf::Packet &packet) {
	packet << static_cast<sf::Uint8>(_packet_type);
}
void Packet_Component::packet_type_write_none(sf::Packet &packet) {
	packet << static_cast<sf::Uint8>(packet_type_none);
}
void Packet_Component::packet_write_uint(sf::Packet &packet, const unsigned int &num) {
	packet << sf::Uint16(num);
}
bool Packet_Component::packet_read_uint(sf::Packet &packet, unsigned int &num) {
	sf::Uint16 temp = 0;
	if (! (packet >> temp)) {
		return false;
	}
	num = temp;
	return true;
}
void Packet_Component::change_packet_type(Packet_Type to) {
	_packet_type = to;
}
void Packet_Component::packet_create(sf::Packet &packet) {
	if (_packet_type == packet_type_none) {
		//ERROR
		packet_type_write_none(packet);
	}
	else {
		//HEAD PACKET WITH UINT8/ENUM FLAG, THEN WRITE AS NORMAL
		packet_type_write(packet);
		packet_write(packet);
	}
}

////ADDITIONAL EXAMPLE CLASS
Player_Name::Player_Name() :
	Packet_Component(packet_type_player_name), _name("Undefined")
{

}
Player_Name::Player_Name(std::string name) :
	Packet_Component(packet_type_player_name), _name(name)
{

}
std::string Player_Name::name() {
	return _name;
}
void Player_Name::name(std::string to) {
	_name = to;
}
void Player_Name::packet_write(sf::Packet &packet) {
	packet << _name;
}
bool Player_Name::packet_read(sf::Packet &packet) {
	return (packet >> _name);
}

////ADDITIONAL CLASS EXAMPLE
Player_ID::Player_ID() :
	Packet_Component(packet_type_player_id_assign), _id(0)
{

}
Player_ID::Player_ID(unsigned int id) :
	Packet_Component(packet_type_player_id_assign), _id(id)
{

}
unsigned int Player_ID::id() {
	return _id;
}
void Player_ID::id(unsigned int to) {
	_id = to;
}
void Player_ID::packet_write(sf::Packet &packet) {
	packet_write_uint(packet, _id);
}
bool Player_ID::packet_read(sf::Packet &packet) {
	return packet_read_uint(packet, _id);
}
