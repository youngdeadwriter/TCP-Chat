#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <string>

#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <math.h>
#include <list>
#include <algorithm>
#include <iterator>
#include <deque>
#include <unordered_map>

#include "myMath.h"
#include "packet_component.h"

//GLOBALS FOR NOW
sf::Font font;
MyMath myMath;

void font_load() {
	if (!font.loadFromFile("Fonts/SourceSansPro-Regular.ttf"))
	{
		std::cout << "Font error\n";
	}
}
///

using namespace std;

static const unsigned int Window_width_default = 800;
static const unsigned int Window_height_default = 600;
static const unsigned int GameProcess_updateThreshold = 100;



enum Direction {
	dir_up, // to minimum y
	dir_right, // to maximum x
	dir_down, // to maximum y
	dir_left, // to minimum x
	dir_invalid //throw error and/or hold null
};

enum Corner {
	corner_topLeft,
	corner_topRight,
	corner_bottomRight,
	corner_bottomLeft
};


enum EventMsg {
	em_None,
	em_Clock_Frame_frame_new,
	em_GameEvent_end,
	em_GameEventChain_end,
	em_GameEventChain_Batch_end,
	em_Event_Capsule_end,
	em_GameEventSchedule_scheduleEmpty,

	em_Visual_ordersComplete,
	em_UnitVisual_ordersComplete,

	em_UI_request_submit,
	em_UI_request_close,
	em_UI_request_skills_act,
	em_UI_request_skills_move,
	em_UI_request_skills_react,
	em_UI_request_wait,

	em_State_action_select_end,
	em_State_action_select_end_wait,
	em_State_skill_select_end,
	em_State_facing_select_end,
	em_State_Maneuver_Build_complete,
	em_State_Stroke_Build_complete,
	em_State_Stroke_Build_reverse,
	em_State_Target_Select_reverse,
	em_State_Target_Select_collect,
	em_State_Area_Confirm_reverse,
	em_State_Area_Confirm_collect,
	em_State_Stroke_Finalize_fail,
	em_State_Stroke_Finalize_succeed,

	em_AnimationLoop_release_reaction,  //?
	em_Last//placeHolder for numberOfEventMsg for now
};

class PairInt {
protected:
	int _x, _y;
public:
	int x();
	void x(int to);
	int y();
	void y(int to);
	void add(PairInt &pair);
	void x_add(int add);
	void y_add(int add);
	void subtract(PairInt &pair);
	void x_subtract(int minus);
	void y_subtract(int minus);
	bool equals(PairInt &compare);

	void x_add_if_greater(int compare);
	void y_add_if_greater(int compare);
	void add_if_greater(PairInt &compare);

	std::string x_toString();
	std::string y_toString();
	std::string xy_toString(std::string seperator);

	PairInt();
	PairInt(int x, int y);
};
PairInt::PairInt() :
	_x(0), _y(0)
{
}
PairInt::PairInt(int x, int y) :
	_x(x), _y(y)
{
}
int PairInt::x() {
	return _x;
}
void PairInt::x(int to) {
	_x = to;
}
int PairInt::y() {
	return _y;
}
void PairInt::y(int to) {
	_y = to;
}
void PairInt::add(PairInt &pair) {
	_x += pair._x;
	_y += pair._y;
}
void PairInt::x_add(int add) {
	_x += add;
}
void PairInt::y_add(int add) {
	_y += add;
}
void PairInt::subtract(PairInt &pair) {
	_x -= pair._x;
	_y -= pair._y;
}
void PairInt::x_subtract(int minus) {
	_x -= minus;
}
void PairInt::y_subtract(int minus) {
	_y -= minus;
}
bool PairInt::equals(PairInt &compare) {
	if (_x == compare._x) {
		if (_y == compare._y) {
			return true;
		}
	}
	return false;
}
std::string PairInt::x_toString() {
	return to_string(_x);
}
std::string PairInt::y_toString() {
	return to_string(_y);
}
std::string PairInt::xy_toString(std::string seperator) {
	std::string temp = x_toString();
	temp += seperator;
	temp += y_toString();
	return temp;
}
void PairInt::x_add_if_greater(int compare) {
	if (compare > _x) {
		_x = compare;
	}
}
void PairInt::y_add_if_greater(int compare) {
	if (compare > _y) {
		_y = compare;
	}
}
void PairInt::add_if_greater(PairInt &compare) {
	x_add_if_greater(compare._x);
	y_add_if_greater(compare._y);
}

class ScreenPosition_Flat {
protected:
public:
	int _x_flat, _y_flat;
	int _layer;
	void add(ScreenPosition_Flat &position);
	void clear();
	ScreenPosition_Flat();
	ScreenPosition_Flat(int x, int y, int layer);
};
ScreenPosition_Flat::ScreenPosition_Flat() :
	_x_flat(0), _y_flat(0), _layer(0)
{
}
ScreenPosition_Flat::ScreenPosition_Flat(int x, int y, int layer) :
	_x_flat(x), _y_flat(y), _layer(layer)
{
}
void ScreenPosition_Flat::add(ScreenPosition_Flat &position) {
	_x_flat += position._x_flat;
	_y_flat += position._y_flat;
	_layer += position._layer;
}
void ScreenPosition_Flat::clear() {
	_x_flat = 0;
	_y_flat = 0;
	_layer = 0;
}

class ScreenPosition {
protected:
public:
	int _x_raw, _y_raw, _z_raw;
	int _layer;
	int _x_flat();
	int _y_flat();
	void add(ScreenPosition &position);
	void add(int x, int y, int z, int layer);
	void clear();
	ScreenPosition find_grounded();
	ScreenPosition();
	ScreenPosition(int x, int y, int z, int layer);
};
ScreenPosition::ScreenPosition() :
	_x_raw(0), _y_raw(0), _z_raw(0), _layer(0)
{
}
ScreenPosition::ScreenPosition(int x, int y, int z, int layer) :
	_x_raw(x), _y_raw(y), _z_raw(z), _layer(layer)
{
}
int ScreenPosition::_x_flat() {
	return _x_raw;
}
int ScreenPosition::_y_flat() {
	return _y_raw - _z_raw;
}
void ScreenPosition::add(ScreenPosition &position) {
	_x_raw += position._x_raw;
	_y_raw += position._y_raw;
	_z_raw += position._z_raw;
	_layer += position._layer;
}
void ScreenPosition::add(int x, int y, int z, int layer) {
	_x_raw += x;
	_y_raw += y;
	_z_raw += z;
	_layer += layer;
}
void ScreenPosition::clear() {
	_x_raw = 0;
	_y_raw = 0;
	_z_raw = 0;
	_layer = 0;
}
ScreenPosition ScreenPosition::find_grounded() {
	//remove z motion
	return ScreenPosition(_x_raw, _y_raw, 0, _layer);
}

class CommentControl {
protected:
	bool _commentsOn, _errorsOn, _labelsOn, _subscriberOn, _notificationsOn, _networkOn;
	int _commentPriority, _errorPriority, _labelPriority, _subscriberPriority, _notificationsPriority, _networkPriority;
	//priorities: 
	//0: critical or expressly forbidden
	//1: initialization errors, file load errors
	//2: program cannot continue
	//4: failure to find expected components
	//5: Likely to cause later errors
	//10: Helpful display info
public:
	void allOff();
	void allOn();
	void subscriberOn();
	void subscriberOff();
	void setAllPriorities(unsigned int priority);


	void sc(string fName, string eText, int priority);
	void sc(string fName, string eText);
	void se(string fName, string eText, int priority);
	void se(string fName, string eText);

	void sl(string name, const char* value, int priority);
	void sl(string name, string value, int priority);
	void sl(string name, bool value, int priority);
	void sl(string name, int value, int priority);
	void sl(string name, unsigned int value, int priority);
	void sl(string name, double value, int priority);
	void sl(int v1, int v2, int priority);

	void sl(string name);
	void sl(string name, const char* value);
	void sl(string name, string value);
	void sl(string name, bool value);
	void sl(string name, int value);
	void sl(string name, unsigned int value);
	void sl(string name, double value);
	void sl(int v1, int v2);
	void sl(vector <string> &stringlist);

	void sl_Mod(string name, int val, int world_min, int world_max);

	void ss_send(unsigned int subject_id, std::string subject_label, unsigned int observer_id, std::string observer_label, EventMsg msg);
	void ss_receive(unsigned int observer_id, std::string observer_label, unsigned int subject_id, std::string subject_label, EventMsg msg);
	void ss_comment(unsigned int subscriber_id, std::string subscriber_label, std::string functionName, std::string comment);
	void ss_error(unsigned int subscriber_id, std::string subscriber_label, std::string functionName, std::string error);

	void nc(string fName, string eText);

	void ntc(std::string name, std::string text, int priority);
	void nte(std::string name, std::string errText, int priority);

	CommentControl();
}cc;
CommentControl::CommentControl() :
	_commentsOn(true), _errorsOn(true), _labelsOn(true), _subscriberOn(true), _notificationsOn(true), _networkOn(true), _commentPriority(10), _errorPriority(10), _labelPriority(10), _subscriberPriority(10), _notificationsPriority(10), _networkPriority(10)
{
}
void CommentControl::allOff() {
	_commentsOn = false;
	_errorsOn = false;
	_labelsOn = false;
	_subscriberOn = false;
	_notificationsOn = false;
	_networkOn = false;
}
void CommentControl::allOn() {
	_commentsOn = true;
	_errorsOn = true;
	_labelsOn = true;
	_subscriberOn = true;
	_notificationsOn = true;
	_networkOn = true;
}
void CommentControl::subscriberOn() {
	_subscriberOn = true;
}
void CommentControl::subscriberOff() {
	_subscriberOn = false;
}
void CommentControl::setAllPriorities(unsigned int priority) {
	_commentPriority = priority;
	_errorPriority = priority;
	_labelPriority = priority;
	_subscriberPriority = priority;
	_notificationsPriority = priority;
	_networkPriority = priority;
}
void CommentControl::sc(string fName, string eText, int priority) {
	if (_commentsOn) {
		if (priority <= _commentPriority) {
			cout << fName << "(), " << eText << endl;
		}
	}
}
void CommentControl::sc(string fName, string eText) {
	if (_commentsOn) {
		if (_commentPriority >= 10) {
			cout << fName << "(), " << eText << endl;
		}
	}
}
void CommentControl::se(string fName, string eText, int priority) {
	if (_errorsOn) {
		if (_errorPriority <= priority) {
			cout << fName << "(), " << eText << endl;
		}
		system("pause");
	}
}
void CommentControl::se(string fName, string eText) {
	if (_errorsOn) {
		if (_errorPriority >= 10) {
			cout << fName << "(), " << eText << endl;
		}
		system("pause");
	}
}
void CommentControl::sl(string name) {
	if (_labelsOn) {
		if (_labelPriority <= 10) {
			cout << name << " : " << endl;
		}
	}
}
void CommentControl::sl(string name, const char* value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, string value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, bool value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			if (value) {
				cout << name << " : " << "TRUE" << endl;
			}
			else {
				cout << name << " : " << "FALSE" << endl;
			}
		}
	}
}
void CommentControl::sl(string name, int value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, unsigned int value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, double value, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(int v1, int v2, int priority) {
	if (_labelsOn) {
		if (priority <= _labelPriority) {
			cout << "Comparison: " << v1 << " : " << v2 << endl;
		}
	}
}
void CommentControl::sl(string name, const char* value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, string value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, bool value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			if (value) {
				cout << name << " : " << "TRUE" << endl;
			}
			else {
				cout << name << " : " << "FALSE" << endl;
			}
		}
	}
}
void CommentControl::sl(string name, int value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, unsigned int value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(string name, double value) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " : " << value << endl;
		}
	}
}
void CommentControl::sl(int v1, int v2) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << "Comparison: " << v1 << " : " << v2 << endl;
		}
	}
}
void CommentControl::sl(vector <string> &stringlist) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			vector <string>::iterator i;
			cout << "List:";
			for (i = stringlist.begin(); i != stringlist.end(); i++) {
				cout << " " << (*i) << ",";
			}
			cout << endl;
		}
	}
}
void CommentControl::sl_Mod(string name, int val, int world_min, int world_max) {
	if (_labelsOn) {
		if (_labelPriority >= 10) {
			cout << name << " Modifier: " << val << ", World Constraint: " << world_min << "-" << world_max << endl;
		}
	}
}
void CommentControl::ss_send(unsigned int subject_id, std::string subject_label, unsigned int observer_id, std::string observer_label, EventMsg msg) {
	if (_subscriberOn) {
		cout << "SubjectID:" << subject_id << " " << subject_label << " sends to ObserverID: " << observer_id << " " << observer_label << " Message#: " << msg << endl;
	}
}
void CommentControl::ss_receive(unsigned int observer_id, std::string observer_label, unsigned int subject_id, std::string subject_label, EventMsg msg) {
	if (_subscriberOn) {
		cout << "ObserverID: " << observer_id << " " << observer_label <<  " receives from SubjectID:" << subject_id << " " << subject_label << " Message#: " << msg << endl;
	}
}
void CommentControl::ss_comment(unsigned int subscriber_id, std::string subscriber_label, std::string functionName, std::string comment) {
	if (_subscriberOn) {
		cout << "Subscriber Comment: " << subscriber_id << " " << subscriber_label << " " << functionName << " " << comment << endl;
	}
}
void CommentControl::ss_error(unsigned int subscriber_id, std::string subscriber_label, std::string functionName, std::string error) {
	if (_subscriberOn) {
		cout << "Subscriber Error: " << subscriber_id << " " << subscriber_label << " " << functionName << " " << error << endl;
	}
}
void CommentControl::nc(string fName, string eText) {
	if (_notificationsOn) {
		cout << fName << "(), " << eText << endl;
	}
}
void CommentControl::ntc(std::string name, std::string text, int priority) {
	if (_networkOn) {
		if (priority <= _networkPriority) {
			std::cout << name << "(), " << text << std::endl;
		}
	}
}
void CommentControl::nte(std::string name, std::string errText, int priority) {
	if (_networkOn) {
		if (priority <= _networkPriority) {
			std::cout << name << "(), " << errText << std::endl;
		}
	}
}



class WindowService {
protected:

public:
	virtual sf::RenderWindow &window() = 0;
	virtual bool openWindow() = 0;
	virtual bool resizeWindow(unsigned int windowWidth, unsigned int windowHeight) = 0;
	virtual PairInt find_window_size() = 0;
	virtual vector <PairInt> window_sizes_allowed() = 0;
	virtual void moveViewSourceBy(int x, int y) = 0;
	virtual void moveViewSource(int x, int y) = 0;
	virtual void slideViewSource(Direction dir) = 0;

	virtual int viewSourceLeftEdge() = 0;
	virtual int viewSourceRightEdge() = 0;
	virtual int viewSourceTopEdge() = 0;
	virtual int viewSourceBottomEdge() = 0;
	virtual int viewSourceWidth() = 0;
	virtual int viewSourceHeight() = 0;
	virtual PairInt viewSourceCenter_asPairInt() = 0;

	virtual PairInt viewSource_findOffset_alignItem_center(int itemWidth, int itemHeight) = 0;
	virtual int viewSource_findOffset_alignItem_right(int itemWidth) = 0;
	virtual int viewSource_findOffset_alignItem_bottom(int itemHeight) = 0;

	virtual int viewportWidthRatio() = 0;
	virtual int viewportHeightRatio() = 0;

	virtual unsigned int milsSinceLastDraw() = 0;
	virtual void setZoomFactor(float to) = 0;

	virtual void view_ui_on() = 0;
	virtual void view_ui_off() = 0;
	virtual unsigned int view_ui_width() = 0;
	virtual unsigned int view_ui_height() = 0;
	virtual unsigned int view_ui_top() = 0;
	virtual unsigned int view_ui_right() = 0;
	virtual unsigned int view_ui_bottom() = 0;
	virtual unsigned int view_ui_left() = 0;
	virtual unsigned int view_ui_center_x() = 0;
	virtual unsigned int view_ui_center_y() = 0;

	virtual ~WindowService();
};
WindowService::~WindowService() {
}

class SL {
protected:
	static WindowService* _windowServ;
public:
	static WindowService* windowServ();

	static void initialize_before();
	static void initialize_after();

	static void initializePhase1(WindowService* windowServ);
	SL();
	~SL();
}sl;
WindowService* SL::_windowServ = nullptr;
SL::SL() {

}
SL::~SL() {
	delete _windowServ;
}
void SL::initialize_before() {
	cc.setAllPriorities(0);
	cc.allOff();
}
void SL::initialize_after() {
	cc.allOn();
}
void SL::initializePhase1(WindowService* windowServ) {
	_windowServ = windowServ;
}

WindowService* SL::windowServ() {
	return _windowServ;
}

class Chat_Line : public Packet_Component {
protected:
	bool _has_header;
	std::string _header;
	std::string _value;
public:
	sf::Text _text;

	void header(std::string header);
	bool has_header();
	void value(std::string value);
	void value_add(std::string toAdd);
	void value_remove();
	int text_width();
	int text_height();
	void update(int x, int y);
	void clear();

	void packet_write(sf::Packet &packet);
	bool packet_read(sf::Packet &packet);
	Chat_Line();
	Chat_Line(const Chat_Line &copy);
	Chat_Line(std::string head, std::string value);
};
Chat_Line::Chat_Line() :
	Packet_Component(packet_type_chat_line_incomplete), _has_header(false), _header(""), _value("")
{

}
Chat_Line::Chat_Line(const Chat_Line &copy) :
	Packet_Component(copy), _has_header(copy._has_header), _header(copy._header), _value(copy._value)
{

}
Chat_Line::Chat_Line(std::string head, std::string value) :
	_has_header(false),_header(""),_value(value)
{
	if (head != "") {
		header(head);
	}
}
void Chat_Line::packet_write(sf::Packet &packet) {
	packet << _has_header << _header << _value;
}
bool Chat_Line::packet_read(sf::Packet &packet) {
	if (! (packet >> _has_header >> _header >> _value)) {
		return false;
	}
	return true;
}
void Chat_Line::header(std::string header) {
	_header = header;
	_has_header = true;
	change_packet_type(packet_type_chat_line_complete);
}
bool Chat_Line::has_header() {
	return _has_header;
}
void Chat_Line::value(std::string value) {
	_value = value;
}
void Chat_Line::value_add(std::string toAdd) {
	_value += toAdd;
}
void Chat_Line::value_remove() {
	if (! _value.empty()) {
		_value.pop_back();
	}
}
int Chat_Line::text_width() {
	return static_cast<int>(_text.getGlobalBounds().width);
}
int Chat_Line::text_height() {
	return static_cast<int>(_text.getGlobalBounds().height);
}
void Chat_Line::update(int x, int y) {
	_text.setFont(font);
	_text.setFillColor(sf::Color::White);
	_text.setCharacterSize(12);
	//_text.setOrigin //?
	_text.setPosition(static_cast<int>(x), static_cast<int>(y));
	_text.setString(_header + ": " + _value);
}
void Chat_Line::clear() {
	_value = "";
}

class Chat_Log {
protected:
	const unsigned int lines_shown_max = 30;
	const int x_rOrigin = 500;
	const int y_rOrigin = 1100;
	const int line_buffer = 4;
	std::vector <Chat_Line> _lines;

	const unsigned int chatters_shown_max = 10;
	const int chatters_x_origin = 1000;
	const int chatters_y_origin = 625;
	std::vector <Chat_Line> _chatters;
public:
	Chat_Line _line_current;

	void add_current(std::string toAdd) {
		_line_current.value_add(toAdd);
	}
	void add_line(Chat_Line line) {
		_lines.push_back(line);
	}
	void backspace_current() {
		_line_current.value_remove();
	}
	void update(std::vector <sf::Text*> &addTo) {
		//UPDATE CHAT LOG ON SCREEN
		std::vector <Chat_Line>::reverse_iterator i;
		unsigned int line_counter = 0;
		unsigned int text_height = 0;
		unsigned int next_base = y_rOrigin;
		for (i = _lines.rbegin(); ((i != _lines.rend()) && (line_counter < lines_shown_max)); i++) {
			text_height = (*i).text_height();
			next_base = myMath.safeSubtract_int(next_base, myMath.safeAdd_int(text_height, line_buffer));
			(*i).update(x_rOrigin, next_base);
			line_counter++;
			addTo.push_back(&(*i)._text);
		}
		//SKIP A LINE AND SHOW THE CURRENT LINE IN PROGRESS
		//text_height = _line_current.text_height();
		unsigned int current_base = myMath.safeAdd_uint(y_rOrigin, (line_buffer * 2));
		_line_current.update(x_rOrigin, current_base);
		addTo.push_back(&_line_current._text);

		//UPDATE CHATTERS ROLE CALL ON SCREEN
		line_counter = 0;
		int origin_cur = chatters_y_origin;
		for (auto j = _chatters.begin(); j != _chatters.end(); j++) {
			text_height = (*j).text_height();
			(*j).update(chatters_x_origin, origin_cur);
			line_counter++;
			addTo.push_back(&(*j)._text);
			origin_cur = myMath.safeAdd_int(origin_cur, myMath.safeAdd_int(text_height, line_buffer));
		}

	}
	Chat_Line submit_current() {
		Chat_Line temp = _line_current;
		_line_current.clear();
		return temp;
	}
	void update_chatters(std::vector <std::string>  & names) {
		_chatters.clear();
		for (auto i = names.begin(); i != names.end(); i++) {
			_chatters.emplace_back((*i), "Connected");
		}
	}
	Chat_Log() {
		_lines.reserve(30);
	}
}chat_log;

class Player : public Packet_Component {
protected:
	unsigned int _id;
	std::string _name;
public:
	unsigned int id() { return _id; }
	void id(unsigned int to) { _id = to; }
	std::string name() { return _name; }
	void name(std::string to) { _name = to; }

	virtual void packet_write(sf::Packet &packet);
	virtual bool packet_read(sf::Packet &packet);
	Player();
	Player(const Player &copy);
	Player(unsigned int id, std::string name);
};
Player::Player() :
	_id(0), _name("Undefined")
{

}
Player::Player(const Player &copy) :
	_id(copy._id), _name(copy._name)
{

}
Player::Player(unsigned int id, std::string name) :
	_id(id), _name(name)
{

}
void Player::packet_write(sf::Packet &packet) {
	packet_write_uint(packet, _id);
	packet << _name;
}
bool Player::packet_read(sf::Packet &packet) {
	//RETURN FALSE IF ANY PORTION FAILS
	//USE THE CONVERSION FUNCTION FOR TRANSFERING UNSIGNED INTS
	if (! packet_read_uint(packet, _id)) {
		return false;
	}

	return packet >> _name;
}

class Player_List : public Packet_Component {
protected:
	std::vector <Player> _players;
public:
	bool player_id_exists(unsigned int player_id);
	Player* player_by_id(bool &okay, unsigned int player_id);
	bool player_add(unsigned int player_id, std::string name);

	void update(Player_List &to);

	virtual void packet_write(sf::Packet &packet);
	virtual bool packet_read(sf::Packet &packet);
	Player_List();
};
Player_List::Player_List() :
	Packet_Component(packet_type_player_list)
{

}
void Player_List::packet_write(sf::Packet &packet) {
	vec_write_component(packet, _players);
}
bool Player_List::packet_read(sf::Packet &packet) {
	return vec_read_component(packet, _players);
}
bool Player_List::player_id_exists(unsigned int player_id) {
	for (auto i = _players.begin(); i != _players.end(); i++) {
		if ((*i).id() == player_id) {
			return true;
		}
	}
	return false;
}
bool Player_List::player_add(unsigned int player_id, std::string name) {
	for (auto i = _players.begin(); i != _players.end(); i++) {
		if ((*i).id() == player_id) {
			return false;
		}
	}

	_players.emplace_back(Player(player_id, name));
	return true;
}
Player* Player_List::player_by_id(bool &okay, unsigned int player_id) {
	okay = false;
	for (auto i = _players.begin(); i != _players.end(); i++) {
		if ((*i).id() == player_id) {
			okay = true;
			return &(*i);
		}
	}
	return nullptr;
}
void Player_List::update(Player_List &to) {
	_players = to._players;
	std::vector <std::string> names;
	for (auto i = _players.begin(); i != _players.end(); i++) {
		names.push_back((*i).name());
	}
	chat_log.update_chatters(names);
}

class Player_Service {
protected:
	unsigned int _player_id_self;

	unsigned int _player_id_available;
public:
	Player_List player_list;
	unsigned int player_id_next();
	bool player_id_self(unsigned int id_self);
	unsigned int player_add(bool &okay, std::string name);
	void update(Player_List &players);

	Player_Service();
}player_service;
Player_Service::Player_Service() :
	_player_id_available(1)
{

}
unsigned int Player_Service::player_id_next() {
	return _player_id_available++;
}
bool Player_Service::player_id_self(unsigned int id_self) {
	if (! player_list.player_id_exists(id_self)) {
		return false;
	}
	_player_id_self = id_self;
	return true;
}
unsigned int Player_Service::player_add(bool &okay, std::string name) {
	okay = false;
	unsigned int id = player_id_next();
	okay = player_list.player_add(id, name);
	return id;
}
void Player_Service::update(Player_List &players) {
	player_list.update(players);
	cc.ntc(__FUNCTION__, "UPDATED", 3);
}

class Connection {
protected:
	std::string _name;

	std::string socket_status_translate(sf::Socket::Status status);
	std::string packet_type_translate(Packet_Type type);
	Packet_Type packet_type_read(sf::Packet &packet);
public:
	void name(std::string to) { _name = to; }
	std::string name() { return _name; }
	virtual bool connect(sf::IpAddress server_address, unsigned short server_listener_port) = 0;
	virtual void packet_receive() = 0;

	virtual bool sync_action(sf::Packet &packet) = 0;

	virtual void update() = 0;

	//ONCE A PACKET HAS BEEN HANDLED AND IS DETERMINED TO BE A SYSTEM EVENT APPLIED THE SAME BY ALL PLAYERS, USE THIS FUNCTION TO APPLY THE EFFECTS TO THE GAME
	bool packet_apply(Packet_Type packet_type, sf::Packet &packet);
	Connection();
	virtual ~Connection();
};
Connection::Connection() :
	_name("Undefined")
{

}
Connection::~Connection() {

}
std::string Connection::socket_status_translate(sf::Socket::Status status) {
	std::string translation = "SOCKET_STATUS: ";
	switch (status) {
		case sf::Socket::Done: {
				translation += "DONE";
				break;
			}
		case sf::Socket::NotReady: {
				translation += "NOTREADY";
				break;
			}
		case sf::Socket::Partial: {
				translation += "PARTIAL";
				break;
			}
		case sf::Socket::Disconnected: {
				translation += "DISCONNECTED";
				break;
			}
		case sf::Socket::Error: {
				translation += "ERROR";
				break;
			}
		default: {
				translation += "UNHANDLED TYPE";
				break;
			}
	}
	return translation;
}
std::string Connection::packet_type_translate(Packet_Type type) {
	std::string translation = "PACKET_TYPE: ";

	switch (type) {
		case packet_type_player_name: {
				translation += "PLAYER_NAME";
				break;
			}
		case packet_type_player_list: {
				translation += "PLAYER LIST";
				break;
			}
		case packet_type_chat_line_incomplete: {
				translation += "CHAT_LINE_INCOMPLETE";
				break;
			}
		case packet_type_chat_line_complete: {
				translation += "CHAT_LINE_COMPLETE";
				break;
			}
		case packet_type_confirm: {
				translation += "CONFIRMATION";
				break;
			}
		case packet_type_deny: {
				translation += "DENIAL";
				break;
			}
		case packet_type_connection_test: {
				translation += "CONNECTION_TEST";
				break;
			}
		case packet_type_request_name: {
				translation += "REQUEST_NAME";
				break;
			}
		case packet_disconnect: {
				translation += "DISCONNECT";
				break;
			}
		default: {
				translation += "UNHANDLED TYPE";
				break;
			}
	}

	return translation;
}
Packet_Type Connection::packet_type_read(sf::Packet &packet) {
	//READS THE FIRST UINT8 FROM THE PACKET WHICH CAN TRANSLATE INTO A PACKET TYPE ENUM DEFINING THE TYPE OF PACKET
	sf::Uint8 type_num = 0;
	if (! (packet >> type_num)) {
		return packet_type_none;
	}
	Packet_Type type = static_cast<Packet_Type>(type_num);
	if ((type < Packet_Type_First) || (type > Packet_Type_Last)) {
		return packet_type_none;
	}

	return type;
}
bool Connection::packet_apply(Packet_Type packet_type, sf::Packet &packet) {
	std::string packet_type_comment = "APPLYING ";
	packet_type_comment += packet_type_translate(packet_type);
	cc.ntc(__FUNCTION__, packet_type_comment, 3);

	switch (packet_type) {
		case packet_type_none: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE!", 0);
				break;
			}
		case packet_type_player_name: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE!", 0);
				break;
			}
		case packet_type_player_id_assign: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE!", 0);
				break;
			}
		case packet_type_player_list: {
				Player_List temp;
				if (! temp.packet_read(packet)) {
					cc.nte(__FUNCTION__, "PACKET READ ERROR", 0);
					return false;
				}
				player_service.update(temp);
				break;
			}
		case packet_type_chat_line_incomplete: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE!", 0);
				break;
			}
		case packet_type_chat_line_complete: {
				Chat_Line temp;
				if (! temp.packet_read(packet)) {
					cc.nte(__FUNCTION__, "PACKET READ ERROR!", 0);
					return false;
				}
				chat_log.add_line(temp);
				break;
			}
		default: {
				cc.nte(__FUNCTION__, "UNHANDLED TYPE!", 0);
				return false;
				break;
			}
	}

	return true;
}

class Client : public Connection {
protected:
	sf::TcpSocket _socket;

	void socket_status_comment(sf::Socket::Status status);
	bool name_send(std::string name);
public:
	virtual bool connect(sf::IpAddress server_address, unsigned short server_listener_port);
	virtual void packet_receive();
	bool packet_handle(sf::Packet &packet);
	virtual bool sync_action(sf::Packet &packet);
	virtual void update();
	Client();
};
Client::Client() {
	_socket.setBlocking(false);
}
void Client::socket_status_comment(sf::Socket::Status status) {
	std::string comment = "CLIENT: ";
	comment += socket_status_translate(status);


	switch (status) {
		case sf::Socket::Done: {
				cc.ntc(__FUNCTION__, comment, 3);
				break;
			}
		case sf::Socket::NotReady:
		case sf::Socket::Partial:
		case sf::Socket::Disconnected:
		case sf::Socket::Error:
		default: {
				cc.nte(__FUNCTION__, comment, 3);
				break;
			}
	}
}
bool Client::name_send(std::string name) {
	cc.ntc(__FUNCTION__, "ATTEMPT", 3);
	Player_Name temp_name(_name);
	sf::Packet temp_packet;
	temp_name.packet_create(temp_packet);

	sf::Socket::Status status = _socket.send(temp_packet);
	socket_status_comment(status);

	if (status != sf::Socket::Done) {
		return false;
	}

	cc.ntc(__FUNCTION__, "NAME RECEIVED BY SERVER", 3);
	return true;
}
bool Client::connect(sf::IpAddress server_address, unsigned short server_listener_port) {
	cc.ntc(__FUNCTION__, "ATTEMPT", 3);
	sf::Socket::Status connection_status = _socket.connect(server_address, server_listener_port);

	socket_status_comment(connection_status);

	if (connection_status == sf::Socket::Done) {
		return name_send(_name);
	}

	return false;
}
void Client::packet_receive() {
	sf::Packet packet;
	sf::Socket::Status status = _socket.receive(packet);
	if (status == sf::Socket::Done) {
		if (! packet_handle(packet)) {
			cc.nte(__FUNCTION__, "PACKET COULD NOT BE HANDLED", 0);
		}
	}
}
bool Client::packet_handle(sf::Packet &packet) {
	Packet_Type type = packet_type_read(packet);
	std::string packet_type_comment = "HANDLING PACKET FROM SERVER ";
	packet_type_comment += packet_type_translate(type);

	switch (type) {
		case packet_type_none: {
				cc.nte(__FUNCTION__, "PACKET IS NOT TYPED", 0);
				return false;
				break;
			}
		case packet_type_player_list: {
				Player_List temp;
				if (! temp.packet_read(packet)) {
					cc.nte(__FUNCTION__, "PACKET READ ERROR!", 0);
					return false;
				}
				player_service.update(temp);
				break;
			}
		case packet_type_player_id_assign: {
				Player_ID temp_id;
				if (! temp_id.packet_read(packet)) {
					cc.nte(__FUNCTION__, "PACKET READ ERROR!", 0);
					return false;
				}
				if (! player_service.player_id_self(temp_id.id())) {
					cc.nte(__FUNCTION__, "ID DOES NOT EXIST IN PLAYER LIST!", 0);
					return false;
				}

				cc.ntc(__FUNCTION__, "PLAYER ID ASSIGNED SUCCESSFULLY", 3);

				break;
			}
		case packet_type_chat_line_incomplete: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE!", 0);
				return false;
				break;
			}
		case packet_type_chat_line_complete: {
				packet_apply(type, packet);
				break;
			}
		default: {
				cc.ntc(__FUNCTION__, "UNHANDLED TYPE, IGNORING", 3);
				break;
			}
	}

	return true;
}
bool Client::sync_action(sf::Packet &packet) {
	cc.ntc(__FUNCTION__, "SENDING", 3);
	sf::Socket::Status status = _socket.send(packet);
	socket_status_comment(status);

	return (status == sf::Socket::Status::Done);
}
void Client::update() {
	packet_receive();
}

class Client_Plug {
protected:
	unsigned int _player_id;
	sf::TcpSocket _socket;
public:
	unsigned int player_id();
	void player_id(unsigned int to);
	sf::TcpSocket & socket();
	Player* player(bool &okay);
	Client_Plug();
	Client_Plug(const Client_Plug &copy);
};
Client_Plug::Client_Plug() {
	_socket.setBlocking(false);
}
Client_Plug::Client_Plug(const Client_Plug &copy) {
	//MUST BE DECLARED BECAUSE 
	_socket.setBlocking(false);
}
unsigned int Client_Plug::player_id() {
	return _player_id;
}
void Client_Plug::player_id(unsigned int to) {
	_player_id = to;
}
sf::TcpSocket & Client_Plug::socket() {
	return _socket;
}
Player* Client_Plug::player(bool &okay) {
	return player_service.player_list.player_by_id(okay, _player_id);
}

class Server : public Connection {
protected:
	std::vector <Client_Plug> _client_plugs;
	sf::TcpListener _listener;

	void socket_status_comment(unsigned int socket_id, sf::Socket::Status status);
public:
	virtual bool connect(sf::IpAddress server_address, unsigned short server_listener_port);
	void accept_connections();

	//GRAB PACKET FROM ANY CLIENT
	virtual void packet_receive();
	//HANDLE PACKETS FROM CLIENTS
	bool packet_handle(Client_Plug &client_plug, sf::Packet &packet);
	//INDIVIDUAL SEND ON A CLIENT PLUG
	bool client_plug_send(Client_Plug &client_plug, sf::Packet &packet);
	//SEND PACKET TO ALL CLIENT PLUGS
	bool distribute_action(sf::Packet &packet);
	//HANDLE OWN PACKETS
	virtual bool sync_action(sf::Packet &packet);
	virtual void update();
	Server();
};
Server::Server() {
	//MUST RESERVE
	_client_plugs.reserve(10);
	//START WITH AN UNCONNECTED PLUG IN PLUGS IN ORDER TO ACCEPT CONNECTIONS
	_client_plugs.emplace_back();
	_listener.setBlocking(false);
}
void Server::socket_status_comment(unsigned int socket_id, sf::Socket::Status status) {
	std::string comment = "SERVER: SOCKET: ";
	comment += socket_id;
	comment += " ";
	comment += socket_status_translate(status);

	switch (status) {
		case sf::Socket::Done: {
				cc.ntc(__FUNCTION__, comment, 3);
				break;
			}
		case sf::Socket::NotReady:
		case sf::Socket::Partial:
		case sf::Socket::Disconnected:
		case sf::Socket::Error:
		default: {
				cc.nte(__FUNCTION__, comment, 3);
				break;
			}
	}
}
bool Server::connect(sf::IpAddress server_address, unsigned short server_listener_port) {
	std::string comment = "ATTEMPT.  SERVER IP: ";
	comment += server_address.toString();
	comment += " LISTENER PORT: ";
	comment += server_listener_port;
	cc.ntc(__FUNCTION__, comment, 3);

	if (server_address == sf::IpAddress::None) {
		cc.ntc(__FUNCTION__, "IP ADDRESS NOT SET", 3);
		return false;
	}

	cc.ntc(__FUNCTION__, "CONNECTING LISTENER", 3);
	sf::Socket::Status status = _listener.listen(server_listener_port);
	socket_status_comment(0, status);

	bool okay = (status == sf::Socket::Done);

	if (! okay) {
		cc.nte(__FUNCTION__, "CONNECTION FAILURE", 0);
		return false;
	}

	//CREATE AND NAME A PLAYER FOR THIS SERVER
	player_service.player_add(okay, _name);
	//SHOW THE SERVER PLAYER IN THE CHATTERS LIST
	player_service.update(player_service.player_list);

	if (! okay) {
		cc.nte(__FUNCTION__, "PLAYER ADD FAILURE", 0);
		return false;
	}

	return okay;
}
void Server::accept_connections() {
	sf::Socket::Status status = _listener.accept(_client_plugs.back().socket());

	if (status == sf::Socket::Done) {
		//FOR NOW, JUST CREATE A BASIC CLIENT PLUG NOT ATTACHED TO ANY PLAYER
		//AFTER CONNECTING SUCCESSFULLY, THE CLIENT WILL SEND OVER IT'S NAME
		//AFTER IT'S NAME IS RECEIVED, A PLAYER IS CREATED WITH THAT NAME AND THE CLIENT_PLUG IT WAS RECEIVED THROUGH IS ASSOCIATED WITH THAT PLAYER
		//THE NEW PLAYER LIST IS SENT TO ALL CLIENTS
		//THEN THE NEW CLIENT IS SENT IT'S PLAYER ID
		//AT THAT POINT, THE CONNECTION WILL BE COMPLETE

		//ADD A NEW EMPTY PLUG TO CONNECT THE NEXT POTENTIAL PLAYER TO
		_client_plugs.emplace_back();
	}
}
void Server::packet_receive() {
	sf::Packet packet;
	sf::Socket::Status status = sf::Socket::Status::Error;

	//ITERATE OVER ALL BUT THE LAST ELEMENT (EMPTY AWAITING CONNECTION)
	//for (auto i = _client_plugs.begin(); i != (--_client_plugs.end()); i++) {
	//	status = (*i).socket().receive(packet);
	//	switch (status) {
	//		case sf::Socket::Status::Error:
	//		case sf::Socket::Status::Partial:
	//		case sf::Socket::Status::Disconnected: {
	//				socket_status_comment((*i).player_id(), status);
	//				break;
	//			}
	//		case sf::Socket::Status::NotReady: {
	//				//THERE WAS NO PACKET TO FIND
	//				break;
	//			}
	//		case sf::Socket::Status::Done: {
	//				if (! packet_handle((*i), packet)) {
	//					cc.nte(__FUNCTION__, "PACKET COULD NOT BE HANDLED", 0);
	//				}
	//			}
	//	}
	//}
	unsigned int iSize = _client_plugs.size();
	myMath.safeDecrement_uint_ref(iSize);
	if (iSize > 0) {
		for (unsigned int i = 0; i < iSize; i++) {
			status = _client_plugs.at(i).socket().receive(packet);
			switch (status) {
				case sf::Socket::Status::Error:
				case sf::Socket::Status::Partial:
				case sf::Socket::Status::Disconnected: {
						socket_status_comment(_client_plugs.at(i).player_id(), status);
						break;
					}
				case sf::Socket::Status::NotReady: {
						//THERE WAS NO PACKET TO FIND
						break;
					}
				case sf::Socket::Status::Done: {
						if (! packet_handle(_client_plugs.at(i), packet)) {
							cc.nte(__FUNCTION__, "PACKET COULD NOT BE HANDLED", 0);
						}
					}
			}
		}
	}
}
bool Server::packet_handle(Client_Plug &client_plug, sf::Packet &packet) {
	Packet_Type type = packet_type_read(packet);
	std::string packet_type_comment = "HANDLING CLIENT PACKET FOR DISTRIBUTION AND APPLICATION";
	packet_type_comment += packet_type_translate(type);
	cc.ntc(__FUNCTION__, packet_type_comment, 3);

	switch (type) {
		case packet_type_none: {
				cc.nte(__FUNCTION__, "PACKET IS NOT TYPED", 0);
				return false;
				break;
			}
		case packet_type_player_name: {
				//THIS IS NOT APPLIED BUT HANDLED BY THE SERVER
				Player_Name temp_name;
				if (! temp_name.packet_read(packet)) {
					cc.nte(__FUNCTION__, "ERROR READING PACKET", 0);
					return false;
				}

				bool okay = false;
				unsigned int player_new_id = player_service.player_add(okay, temp_name.name());

				if (! okay) {
					cc.nte(__FUNCTION__, "ERROR ADDING PLAYER", 0);
					return false;
				}
				else {
					std::string comment = "PLAYER ADDED # ";
					comment += player_new_id;
					comment += " NAMED ";
					comment += temp_name.name();
					cc.ntc(__FUNCTION__, comment, 3);
				}

				//NOW CONNECT THE SOCKET (CREATED WHILE ACCEPTING CONNECTIONS) WITH THIS PLAYER ID
				client_plug.player_id(player_new_id);

				//NOW SEND BACK THE UPDATED PLAYER LIST

				//CREATE PLAYER LIST PACKET
				sf::Packet packet_player_list;
				player_service.player_list.packet_create(packet_player_list);

				//SEND TO ALL PLAYERS
				distribute_action(packet_player_list);

				//THEN SEND OVER THE ID FOR THE CLIENT TO USE
				Player_ID temp_id(player_new_id);
				sf::Packet packet_player_id;
				temp_id.packet_create(packet_player_id);

				//SEND ONLY TO THE PLAYER WHO WAS JUST ADDED, USE THE FOUND SOCKET

				sf::Socket::Status status = client_plug.socket().send(packet_player_id);
				if (status != sf::Socket::Status::Done) {
					cc.nte(__FUNCTION__, "ERROR SENDING PLAYER ID", 0);
					return false;
				}

				cc.ntc(__FUNCTION__, "PLAYER CONNECTED SUCCESSFULLY", 3);

				break;
			}
		case packet_type_player_list: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE", 0);
				break;
			}
		case packet_type_chat_line_incomplete: {
				//THE SERVER TAKES THE NAMELESS CHAT_LINE, ADDS THE CLIENT NAME, THEN DISTRIBUTES IT TO EVERYONE
				Chat_Line chat_line;
				if (! chat_line.packet_read(packet)) {
					cc.nte(__FUNCTION__, "ERROR READING PACKET", 0);
					return false;
				}

				bool okay = false;
				Player* player = client_plug.player(okay);
				if (! okay) {
					cc.nte(__FUNCTION__, "ERROR FINDING CLIENT BY ID", 0);
					return false;
				}

				//ADD PLUG NAME AS HEADER TO CHAT LINE
				chat_line.header(player->name());
				//REPACKAGE AS A PACKET
				sf::Packet temp;
				chat_line.packet_create(temp);

				//DISTRIBUTE THE FINISHED PACKET
				return distribute_action(temp);
				break;
			}
		case packet_type_chat_line_complete: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE", 0);
				break;
			}
		default: {
				cc.ntc(__FUNCTION__, "UNHANDLED TYPE, IGNORING", 3);
				break;
			}
	}

	return true;
}
bool Server::client_plug_send(Client_Plug &client_plug, sf::Packet &packet) {
	std::string comment = "SENDING TO PLAYER ";

	bool okay = false;
	Player* player = client_plug.player(okay);
	if (! okay) {
		comment += "UNKNOWN PLAYER";
	}
	else {
		comment += player->name();
	}
	cc.ntc(__FUNCTION__, comment, 3);

	sf::Socket::Status status = client_plug.socket().send(packet);
	socket_status_comment(client_plug.player_id(), status);
	if (status != sf::Socket::Status::Done) {
		//HANDLE SENDS THAT AREN'T RECEIVED
		return false;
	}


	return true;
}
bool Server::distribute_action(sf::Packet &packet) {
	bool all_okay = true;

	unsigned int iSize = 0;
	iSize = _client_plugs.size();
	myMath.safeDecrement_uint_ref(iSize);
	if (iSize > 0) {
		for (unsigned int i = 0; i < iSize; i++) {
			bool send_okay = client_plug_send(_client_plugs.at(i), packet);
			if (! send_okay) {
				all_okay = false;
			}
		}
	}

	/*for (auto i = _client_plugs.begin(); i != (--_client_plugs.end()); i++) {
	bool send_okay = client_plug_send((*i), packet);
	if (! send_okay) {
	all_okay = false;
	}
	}*/

	//THEN HANDLE THE PACKET ITSELF
	Packet_Type type = packet_type_read(packet);
	return packet_apply(type, packet);
}
bool Server::sync_action(sf::Packet &packet) {
	//HANDLE PACKETS COMING FROM THIS COMPUTER SO THAT THEY ARE READY FOR DISTRIBUTION TO ALL

	Packet_Type type = packet_type_read(packet);
	std::string packet_type_comment = "HANDLING OWN PACKET FOR DISTRIBUTION ";
	packet_type_comment += packet_type_translate(type);
	cc.ntc(__FUNCTION__, packet_type_comment, 3);

	switch (type) {
		case packet_type_none: {
				cc.nte(__FUNCTION__, "PACKET IS NOT TYPED", 0);
				return false;
				break;
			}
		case packet_type_player_name: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE", 0);
				break;
			}
		case packet_type_player_list: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE", 0);
				break;
			}
		case packet_type_chat_line_incomplete: {
				//THE SERVER TAKES THE NAMELESS CHAT_LINE, ADDS THE CLIENT NAME, THEN DISTRIBUTES IT TO EVERYONE
				Chat_Line chat_line;
				if (! chat_line.packet_read(packet)) {
					cc.nte(__FUNCTION__, "ERROR READING PACKET", 0);
					return false;
				}

				//ADD PLUG NAME AS HEADER TO CHAT LINE
				chat_line.header(_name);
				//REPACKAGE AS A PACKET
				sf::Packet temp;
				chat_line.packet_create(temp);

				//DISTRIBUTE THE FINISHED PACKET
				return distribute_action(temp);
				break;
			}
		case packet_type_chat_line_complete: {
				cc.nte(__FUNCTION__, "SHOULD NOT RECEIVE", 0);
				break;
			}
		default: {
				cc.ntc(__FUNCTION__, "UNHANDLED TYPE, IGNORING", 3);
				break;
			}
	}

	return true;
}
void Server::update() {
	accept_connections();
	packet_receive();
}

class Network {
protected:
	Connection* _connection;
	sf::IpAddress _server_address;
	unsigned short _server_listener_port;
	bool connection_verify();
public:
	bool initialize(std::string name, bool hosting, sf::IpAddress server_address, unsigned short server_listener_port);
	bool connect();
	void update();
	bool sync_action(sf::Packet &packet);
	Network();
}network;
Network::Network() :
	_connection(nullptr)
{

}
bool Network::connection_verify() {
	if (_connection == nullptr) {
		cc.nte(__FUNCTION__, "CONNECTION IS NULL OR NOT SET", 1);
		return false;
	}
	return true;
}
bool Network::initialize(std::string name, bool hosting, sf::IpAddress server_address, unsigned short server_listener_port) {
	_server_address = server_address;
	_server_listener_port = server_listener_port;

	if (hosting) {
		_connection = new Server();
	}
	else {
		_connection = new Client();
	}

	if (connection_verify()) {
		_connection->name(name);
	}

	return true;
}
bool Network::connect() {
	if (! connection_verify()) {
		return false;
	}
	return _connection->connect(_server_address, _server_listener_port);
}
void Network::update() {
	if (! connection_verify()) {
		return;
	}

	_connection->update();
}
bool Network::sync_action(sf::Packet &packet) {
	if (! connection_verify()) {
		return false;
	}

	return _connection->sync_action(packet);
}

class GameProcess {
protected:
	unsigned int _timeSinceLastUpdate;
	unsigned int _updateThreshold;

	vector <sf::Text*> allDrawables;
	vector <sf::Text*>::iterator iAll;

public:
	void input_find();
	void network_talk();
	void run_graphics();
	void start_gameLoop();
	GameProcess();
	~GameProcess();
}gameProcess;
GameProcess::GameProcess() :
	_timeSinceLastUpdate(0), _updateThreshold(GameProcess_updateThreshold)
{
}
GameProcess::~GameProcess() {
}
void GameProcess::input_find() {
	sf::Keyboard::Key _input_key = sf::Keyboard::Unknown;
	bool _lock = false;

	sf::Event event;
	// while there are pending events...
	while (sl.windowServ()->window().pollEvent(event)) {
		//THERE MAY ME OTHER PENDING BESIDES KEYBOARD PRESSES
		//WE ONLY WANT TO USE THE POLLEVENT FOR USER INPUT AND A FEW WELL-DEFINED SYSTEM REQUUESTS

		//FIRST, CHECK SYSTEM EVENTS THAT CLOSE THIS WINDOW/GAME, OR PERFORM OTHER SERVICES OUTSIDE OF THE GAME LOGIC
		//FOR INSTANCE, HANDLE THE CLOSE WINDOW FUNCTION BEFORE GETTING INTO USER INPUT
		if (event.type ==  sf::Event::Closed) {
			// window closed
			sl.windowServ()->window().close();
			return;
		}

		//KEYPRESSED AND TEXTENTERED ARE TWO SEPERATE EVENTS THAT WILL POLL DIFFERENTLY ON THE SAME PASS THROUGH HERE

		/*if (event.type == sf::Event::KeyPressed) {
		_input_key = event.key.code;
		switch (_input_key) {
		case sf::Keyboard::Return: {
		chat_log.submit_current();
		break;
		}
		case sf::Keyboard::BackSpace: {
		chat_log.backspace_current();
		break;
		}
		default: {
		break;
		}
		}
		}*/

		if (event.type == sf::Event::TextEntered)
		{
			if (event.text.unicode < 0x80) {
				std::string key_string = "";
				key_string = (char)event.text.unicode;
				//IGNORE SPECIAL CHARACTERS
				if (! key_string.empty()) {
					switch (key_string.at(0)) {
						case '\r': {
								Chat_Line to_submit = chat_log.submit_current();
								sf::Packet packet;
								to_submit.packet_create(packet);
								network.sync_action(packet);
								break;
							}
						case '\b': {
								chat_log.backspace_current();
								break;
							}
						default: {
								chat_log._line_current.value_add(key_string);
								break;
							}
					}
				}
			}
		}


		sf::Keyboard::Key _input_key = sf::Keyboard::Unknown;

	}
}
void GameProcess::network_talk() {
	network.update();
}
void GameProcess::run_graphics() {
	sl.windowServ()->window().clear();

	//ADD GAME CLOCK TIME SINCE LAST UPDATE TO _timeSinceLastUpdate
	_timeSinceLastUpdate = myMath.safeAdd_uint(_timeSinceLastUpdate, sl.windowServ()->milsSinceLastDraw());
	//cout << _timeSinceLastUpdate << endl;

	//
	if (_timeSinceLastUpdate >= _updateThreshold) {
		allDrawables.clear();
		_timeSinceLastUpdate = 0;
		chat_log.update(allDrawables);
	}
	//

	for (iAll = allDrawables.begin(); iAll != allDrawables.end(); iAll++) {
		sl.windowServ()->window().draw(*(*iAll));
	}

	sl.windowServ()->window().display();
}
void GameProcess::start_gameLoop() {
	while (sl.windowServ()->window().isOpen()) {
		input_find();
		network_talk();
		run_graphics();
	}
}

class WindowServiceProvider : public WindowService {
private:
	string _title;
	sf::RenderWindow _window;
	unsigned int _windowWidth, _windowHeight;
	sf::View _view;
	unsigned int _viewSourceWidth, _viewSourceHeight;
	int _x_viewSourceCenter, _y_viewSourceCenter;

	//viewport (portion of window the view is shown on) features:
	unsigned int _x_aspectRatio, _y_aspectRatio;

	//PERCENT OF THE TOTAL VIEW SOURCE AREA BY WHICH TO MOVE WHEN SLIDING IN A DIRECTION
	float _view_source_slide_ratio;

	float _zoomFactor;

	void updateViewport();
	void updateViewSource();

	sf::Clock _drawClock;

	int viewSource_x_min();
	int viewSource_x_max();
	int viewSource_y_min();
	int viewSource_y_max();
	float invertedZoomFactor();

	sf::View _view_ui;
	unsigned int _view_ui_width;
	unsigned int _view_ui_height;
	unsigned int _view_ui_top;
	unsigned int _view_ui_right;
	unsigned int _view_ui_bottom;
	unsigned int _view_ui_left;
	unsigned int _view_ui_center_x;
	unsigned int _view_ui_center_y;
	void view_ui_update();

	vector <PairInt> _window_sizes_allowed;
	bool window_size_verify(PairInt size);

public:
	virtual sf::RenderWindow &window();
	virtual bool openWindow();
	virtual bool resizeWindow(unsigned int windowWidth, unsigned int windowHeight);
	virtual PairInt find_window_size();
	virtual vector <PairInt> window_sizes_allowed();
	virtual void moveViewSourceBy(int x, int y);
	virtual void moveViewSource(int x, int y);
	virtual void slideViewSource(Direction dir);

	virtual int viewSourceLeftEdge();
	virtual int viewSourceRightEdge();
	virtual int viewSourceTopEdge();
	virtual int viewSourceBottomEdge();
	virtual int viewSourceWidth();
	virtual int viewSourceHeight();
	virtual PairInt viewSourceCenter_asPairInt();
	//BORDER VISUAL -SHOULD- BE HANDLING THE TYPE OF VISUALS THAT MIGHT NEED THESE FUNCTIONS, BUT THERE'S NO REASON NOT TO HAVE THEM HERE
	virtual PairInt viewSource_findOffset_alignItem_center(int itemWidth, int itemHeight);
	virtual int viewSource_findOffset_alignItem_right(int itemWidth);
	virtual int viewSource_findOffset_alignItem_bottom(int itemHeight);
	//

	virtual int viewportWidthRatio();
	virtual int viewportHeightRatio();

	virtual unsigned int milsSinceLastDraw();
	virtual void setZoomFactor(float to);

	virtual void view_ui_on();
	virtual void view_ui_off();
	virtual unsigned int view_ui_width();
	virtual unsigned int view_ui_height();
	virtual unsigned int view_ui_top();
	virtual unsigned int view_ui_right();
	virtual unsigned int view_ui_bottom();
	virtual unsigned int view_ui_left();
	virtual unsigned int view_ui_center_x();
	virtual unsigned int view_ui_center_y();

	WindowServiceProvider();
	WindowServiceProvider(unsigned int windowWidth, unsigned int windowHeight);
};
WindowServiceProvider::WindowServiceProvider() :
	_title("Window Title"), _windowWidth(Window_width_default), _windowHeight(Window_height_default), _viewSourceWidth(Window_width_default), _viewSourceHeight(Window_height_default), _x_viewSourceCenter(1000), _y_viewSourceCenter(1000), _x_aspectRatio(4), _y_aspectRatio(3), _view_source_slide_ratio(0.05), _zoomFactor(1), _view_ui_width(Window_width_default), _view_ui_height(Window_height_default), _view_ui_top(0), _view_ui_right(0), _view_ui_bottom(0), _view_ui_left(0), _view_ui_center_x(0), _view_ui_center_y(0)
{
	_window_sizes_allowed ={ PairInt(600,400), PairInt(800,600), PairInt(1280,800) };
}
WindowServiceProvider::WindowServiceProvider(unsigned int windowWidth, unsigned int windowHeight) :
	_title("Window Title"), _windowWidth(windowWidth), _windowHeight(windowHeight), _viewSourceWidth(Window_width_default), _viewSourceHeight(Window_height_default), _x_viewSourceCenter(1000), _y_viewSourceCenter(1000), _x_aspectRatio(4), _y_aspectRatio(3), _view_source_slide_ratio(0.05), _zoomFactor(1), _view_ui_width(windowWidth), _view_ui_height(windowHeight), _view_ui_top(0), _view_ui_right(0), _view_ui_bottom(0), _view_ui_left(0), _view_ui_center_x(0), _view_ui_center_y(0)
{
	_window_sizes_allowed ={ PairInt(600,400), PairInt(800,600), PairInt(1280,800) };
}
bool WindowServiceProvider::window_size_verify(PairInt size) {
	for (auto i = _window_sizes_allowed.begin(); i != _window_sizes_allowed.end(); i++) {
		if ((*i).equals(size)) {
			return true;
		}
	}
	return false;
}
void WindowServiceProvider::view_ui_update() {
	//MAINTAIN A STATIC SIZE AND CENTER FOR UI VIEWSOURCE, THIS SHOULD BE THE DEFAULT RESOLUTION FOR THE GAME
	_view_ui_width = Window_width_default;
	_view_ui_height = Window_height_default;
	_view_ui_top = 0;
	_view_ui_right = _view_ui_width;
	_view_ui_bottom = _view_ui_height;
	_view_ui_left = 0;
	_view_ui_center_x = (_view_ui_width / 2);
	_view_ui_center_y = (_view_ui_height / 2);

	//SET VIEW SOURCE SIZE TO THIS STATIC NUMBER AND CENTER TO THE MIDDLE XY COORDINATE
	_view_ui.setSize(static_cast<float>(_view_ui_width), static_cast<float>(_view_ui_height));
	_view_ui.setCenter((static_cast<float>(_view_ui_width / 2)), (static_cast<float>(_view_ui_height / 2)));

	//STRETCH VIEWPORT OF UI TO FIT FULL DIMENSIONS OF WINDOW
	_view_ui.setViewport(sf::FloatRect(0, 0, 1, 1));
	//_view_ui.zoom(0.5);

}
void WindowServiceProvider::updateViewport() {

	float ratioUnit = (float)_windowHeight / (float)_y_aspectRatio;
	float _viewportHeight = ratioUnit * (float)_y_aspectRatio;
	float _viewportWidth = ratioUnit * (float)_x_aspectRatio;

	float _viewportPortionWidth = _viewportWidth / (float)_windowWidth;
	float _viewportPortionHeight = _viewportHeight / (float)_windowHeight;

	float _xOffset = (float)_windowWidth - _viewportWidth;
	float _yOffset = (float)_windowHeight - _viewportHeight;

	float _widthPortionOffset = (_xOffset / 2) / _windowWidth;
	float _heightPortionOffset = (_yOffset / 2) / _windowHeight;

	_view.setViewport(sf::FloatRect(_widthPortionOffset, _heightPortionOffset, _viewportPortionWidth, _viewportPortionHeight));
}
void WindowServiceProvider::updateViewSource() {
	_view.setSize(sf::Vector2f(((float)_viewSourceWidth * _zoomFactor), ((float)_viewSourceHeight * _zoomFactor)));
	_view.setCenter(sf::Vector2f((float)_x_viewSourceCenter, (float)_y_viewSourceCenter));
	_window.setView(_view);
}
int WindowServiceProvider::viewSource_x_min() {
	//return static_cast<int>(sl.worldServ()->center().x() - ((_viewSourceWidth * invertedZoomFactor()) / 2));
	return 0;
}
int WindowServiceProvider::viewSource_x_max() {
	//return static_cast<int>(sl.worldServ()->center().x() + ((_viewSourceWidth * invertedZoomFactor()) / 2));
	return 2000;
}
int WindowServiceProvider::viewSource_y_min() {
	//return static_cast<int>(sl.worldServ()->center().y() - ((_viewSourceHeight * invertedZoomFactor()) / 2));
	return 0;
}
int WindowServiceProvider::viewSource_y_max() {
	//return static_cast<int>(sl.worldServ()->center().y() + ((_viewSourceHeight * invertedZoomFactor()) / 2));
	return 2000;
}
float WindowServiceProvider::invertedZoomFactor() {
	//DIVIDE 1 BY ZOOMFACTOR TO INVERT THE MULTIPLIER
	return myMath.safeDivide_float(1, _zoomFactor);
}
sf::RenderWindow & WindowServiceProvider::window() {
	return _window;
}
bool WindowServiceProvider::openWindow() {
	_window.create(sf::VideoMode(_windowWidth, _windowHeight), _title, sf::Style::Close);
	//_view.setSize(_windowWidth,_windowHeight);
	_view.setSize((float)_viewSourceWidth, (float)_viewSourceHeight);

	//_x_viewSourceCenter = sl.worldServ()->center().x();
	//_y_viewSourceCenter = sl.worldServ()->center().y();
	_x_viewSourceCenter = 900;
	_y_viewSourceCenter = 900;

	_view.setCenter(sf::Vector2f((float)_x_viewSourceCenter, (float)_y_viewSourceCenter));

	//_view.setSize(sf::Vector2f( (float)_windowWidth, (float)_windowHeight) );
	//resizeView();
	updateViewport();
	_window.setView(_view);

	view_ui_update();

	return true;
}
bool WindowServiceProvider::resizeWindow(unsigned int windowWidth, unsigned int windowHeight) {
	if (window_size_verify(PairInt(windowWidth, windowHeight))) {
		cc.se(__FUNCTION__, "WINDOW SIZE NOT IN ALLOWED LIST", 0);
		return false;
	}

	sf::Vector2f prevViewCenter = _view.getCenter();
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_window.setSize(sf::Vector2u(_windowWidth, _windowHeight));
	_view.setCenter(prevViewCenter);
	updateViewport();
	updateViewSource();
	//_view.reset(sf::FloatRect( (float)sl.worldServ()->center().x(), (float)sl.worldServ()->center().y(), (float)_windowWidth, (float)_windowHeight) );
	//_view.setCenter( sf::Vector2f( (float)sl.worldServ()->center().x(), (float)sl.worldServ()->center().y() ) );
	//_view.setSize(sf::Vector2f( (float)_windowWidth, (float)_windowHeight) );
	//resizeView();
	_window.setView(_view);
	return true;
}
PairInt WindowServiceProvider::find_window_size() {
	return PairInt(_windowWidth, _windowHeight);
}
vector <PairInt> WindowServiceProvider::window_sizes_allowed() {
	return _window_sizes_allowed;
}
void WindowServiceProvider::moveViewSourceBy(int x, int y) {
	myMath.add_withinRange_intRef(_x_viewSourceCenter, x, viewSource_x_min(), viewSource_x_max());
	myMath.add_withinRange_intRef(_y_viewSourceCenter, y, viewSource_y_min(), viewSource_y_max());
	//_x_viewSourceCenter += x;
	//_y_viewSourceCenter += y;
	_view.setCenter((float)_x_viewSourceCenter, (float)_y_viewSourceCenter);
	_window.setView(_view);
}
void WindowServiceProvider::moveViewSource(int x, int y) {
	myMath.set_withinRange_intRef(_x_viewSourceCenter, x, viewSource_x_min(), viewSource_x_max());
	myMath.set_withinRange_intRef(_y_viewSourceCenter, y, viewSource_y_min(), viewSource_y_max());
	//_x_viewSourceCenter = x;
	//_y_viewSourceCenter = y;
	_view.setCenter((float)_x_viewSourceCenter, (float)_y_viewSourceCenter);
	_window.setView(_view);
}
void WindowServiceProvider::slideViewSource(Direction dir) {
	int x = 0;
	int y = 0;
	switch (dir) {
		case dir_up: {
				int y = (-1 * static_cast<int>(_viewSourceHeight * _view_source_slide_ratio));
				break;
			}
		case dir_right: {
				int x = static_cast<int>(_viewSourceWidth * _view_source_slide_ratio);
				break;
			}
		case dir_down: {
				int y = static_cast<int>(_viewSourceHeight * _view_source_slide_ratio);
				break;
			}
		case dir_left: {
				int x = (-1 * static_cast<int>(_viewSourceWidth * _view_source_slide_ratio));
				break;
			}

	}

	moveViewSourceBy(x, y);
}
int WindowServiceProvider::viewSourceLeftEdge() {
	//for conversion from float to int
	return _x_viewSourceCenter - (_viewSourceWidth / 2);
	//return (int)_view.getViewport().left + 0.5;
}
int WindowServiceProvider::viewSourceRightEdge() {
	return _x_viewSourceCenter + (_viewSourceWidth / 2);
	//return ( (int)_view.getViewport().left + 0.5) + ( (int)_view.getViewport().width + 0.5);
}
int WindowServiceProvider::viewSourceTopEdge() {
	return _y_viewSourceCenter - (_viewSourceHeight / 2);
	//return (int)_view.getViewport().top + 0.5;
}
int WindowServiceProvider::viewSourceBottomEdge() {
	return _y_viewSourceCenter + (_viewSourceHeight / 2);
	//return ( (int)_view.getViewport().top + 0.5) + ( (int)_view.getViewport().height + 0.5);
}
int WindowServiceProvider::viewSourceWidth() {
	return _viewSourceWidth;
}
int WindowServiceProvider::viewSourceHeight() {
	return _viewSourceHeight;
}
PairInt WindowServiceProvider::viewSourceCenter_asPairInt() {
	return PairInt((int)_view.getCenter().x, (int)_view.getCenter().y);
}
PairInt WindowServiceProvider::viewSource_findOffset_alignItem_center(int itemWidth, int itemHeight) {
	PairInt toReturn(viewSourceCenter_asPairInt());
	toReturn.x_subtract(itemWidth / 2);
	toReturn.y_subtract(itemHeight / 2);
	return toReturn;
}
int WindowServiceProvider::viewSource_findOffset_alignItem_right(int itemWidth) {
	return viewSourceRightEdge() - itemWidth;
}
int WindowServiceProvider::viewSource_findOffset_alignItem_bottom(int itemHeight) {
	return viewSourceBottomEdge() - itemHeight;
}
int WindowServiceProvider::viewportWidthRatio() {
	return (int)_view.getViewport().width;
}
int WindowServiceProvider::viewportHeightRatio() {
	return (int)_view.getViewport().height;
}
unsigned int WindowServiceProvider::milsSinceLastDraw() {
	//return static_cast<unsigned int>(_drawClock.restart().asMilliseconds() );
	return static_cast<unsigned int>(_drawClock.restart().asMicroseconds());
}
void WindowServiceProvider::setZoomFactor(float to) {
	//THIS CURRENTLY JUST UPDATES THE ZOOM FACTOR AND THEN THE VIEWSOURCE
	//BUT EVENTUALLY ANYTIME THE VIEWSOURCE IS CHANGED THIS NEEDS TO BE FACTORED IN AS WELL
	_zoomFactor = to;
	updateViewSource();
}
void WindowServiceProvider::view_ui_on() {
	_window.setView(_view_ui);
	//view_ui_update();
}
void WindowServiceProvider::view_ui_off() {
	_window.setView(_view);
}
unsigned int WindowServiceProvider::view_ui_width() {
	return _view_ui_width;
}
unsigned int WindowServiceProvider::view_ui_height() {
	return _view_ui_height;
}
unsigned int WindowServiceProvider::view_ui_top() {
	return _view_ui_top;
}
unsigned int WindowServiceProvider::view_ui_right() {
	return _view_ui_right;
}
unsigned int WindowServiceProvider::view_ui_bottom() {
	return _view_ui_bottom;
}
unsigned int WindowServiceProvider::view_ui_left() {
	return _view_ui_left;
}
unsigned int WindowServiceProvider::view_ui_center_x() {
	return _view_ui_center_x;
}
unsigned int WindowServiceProvider::view_ui_center_y() {
	return _view_ui_center_y;
}

std::string name_enter(bool ask) {
	std::string name = "Auto-Name Server";
	if (ask) {
		std::cout << "Enter name: ";
		std::cin >> name;
	}
	return name;
}

bool host_enter(bool ask) {
	sf::IpAddress local = sf::IpAddress::getLocalAddress();
	sf::IpAddress pub = sf::IpAddress::getPublicAddress();

	if (ask) {
		unsigned int host = -1;
		std::cout << "\nSelect Connection Type (0 = host, others = client) : ";
		std::cin >> host;
		if (host == 0) {
			return true;
		}
		else {
			return false;
		}
	}

	return true;
}

sf::IpAddress address_enter(bool ask, bool local) {
	sf::IpAddress ip_local = sf::IpAddress::getLocalAddress();
	sf::IpAddress ip_public = sf::IpAddress::getPublicAddress();

	if (! ask) {
		if (local) {
			return ip_local;
		}
		return ip_public;
	}

	std::cout << "Local ip address for this computer: " << ip_local << std::endl;
	std::cout << "Public ip address for this computer: " << ip_public << std::endl;

	std::string ip_entered = "";
	std::cout << "Enter server ip address: ";
	std::cin >> ip_entered;

	return sf::IpAddress(ip_entered);
}

unsigned short port_enter(bool ask) {
	if (! ask) {
		return 5300;
	}

	unsigned short port = 0;
	std::cout << "Enter the server's listener port: ";
	std::cin >> port;

	return port;
}

bool start_network(bool ask) {
	std::string name = name_enter(ask);
	bool host = host_enter(ask);
	sf::IpAddress server_address = address_enter(ask, true);
	unsigned short server_listener_port = port_enter(ask);

	network.initialize(name, host, server_address, server_listener_port);

	bool retry = true;
	while (retry) {
		std::cout << "Trying to connect to " << server_address << ": " << server_listener_port << std::endl;
		if (! network.connect()) {
			std::cout << "Connection Failure.  Retrying...";
			/*std::cout << "Connection Failure.  Press any key to retry.\n";
			std::string temp;
			std::cin >> temp;*/
		}
		else {
			retry = false;
		}
	}

	return true;
}

int main()
{
	font_load();

	sl.initializePhase1(new WindowServiceProvider(Window_width_default, Window_height_default));

	sl.windowServ()->openWindow();

	sl.windowServ()->moveViewSource(900, 900);

	Chat_Line intro;
	intro.value("Start typing!");

	chat_log.add_line(intro);

	start_network(true);

	gameProcess.start_gameLoop();

	system("pause");

	return 0;
}


//class State_Distribute {
//protected:
//	Connection* _connection;
//	Packet_Type packet_type_read(sf::Packet &packet);
//
//	bool connection_verify() { return _connection != nullptr; }
//public:
//	//Chat chat;
//
//	void packet_handle(unsigned int client_id, sf::Packet &packet);
//	bool packet_distribute(sf::Packet &packet);
//	void update();
//
//	State_Distribute();
//	State_Distribute(Connection& connection);
//}state_distribute(connection);
//State_Distribute::State_Distribute() :
//	_connection(nullptr)
//{
//	std::cout << "Do not use default constructor\n";
//}
//State_Distribute::State_Distribute(Connection& connection) :
//	_connection(&connection)
//{
//
//}
//void State_Distribute::packet_handle(unsigned int client_id, sf::Packet &packet) {
//
//	if (! connection_verify()) {
//		return;
//	}
//	//SOME WILL NEED TO BE REDISTRIBUTED.  CHECK IF HOST AND IF SO, DISTRIBUTE SOME BEFORE PROCESSING
//	//Packet_Type packet_type_test = packet_type_read(packet);
//
//	switch (packet_type_read(packet)) {
//		case packet_type_player_name: {
//				if (_connection->host()) {
//					std::string name = "";
//					packet >> name;
//					Chat_Line announcement;
//					announcement.value(name + " has entered!");
//					chat_log.add_line(announcement);
//					_connection->server_self.client_name(client_id, name);
//				}
//			}
//		case packet_type_chat_message: {
//				//STORE THE CHAT LINE IN A TEMP OBJECT
//				Chat_Line line_temp;
//				line_temp.packet_read(packet);
//
//				if (_connection->host()) {
//					//THE NAME TO PREFACE THIS LINE WITH
//					sf::String name_temp;
//
//					if (client_id == 0) {
//						name_temp = _connection->name() + ": ";
//						line_temp.value_preface(name_temp);
//					}
//					else {
//						//HOST NEEDS TO FIND THE CLIENT'S NAME AND PREFACE THE CHAT LINE WITH IT
//						//FIND THE CLIENT BY ID
//						Client* client_temp = nullptr;
//						client_temp = connection.server_self.client_by_id(client_id);
//						//IF THE CLIENT IS FOUND, STORE IT'S NAME AND PREFACE THE CHAT LINE WITH IT
//						if (client_temp != nullptr) {
//							name_temp = client_temp->name() + ": ";
//							line_temp.value_preface(name_temp);
//
//							//THEN, HAVING COMPLETED THE CHAT_LINE, IT SHOULD DISTRIBUTE THE NEW PACKET TO ALL CLIENTS
//							//CREATE A NEW PACKET
//
//						}
//						else {
//							line_temp.value("SERVER: ERROR, CLIENT ID NOT FOUND OR IS NULL");
//						}
//					}
//
//					//CREATE A TEMP PACKET FOR THE DISTRIBUTION OF A MANAGED PACKET
//					sf::Packet packet_to_distribute;
//					line_temp.packet_create(packet_to_distribute);
//
//					//SEND MANAGED PACKET TO ALL CLIENTS
//					_connection->send(packet_to_distribute);
//				}
//
//				//NOW ALL COMPUTERS (SERVER AND CLIENT) SHOULD ADD THE MANAGED LINE
//				chat_log.add_line(line_temp);
//				break;
//			}
//		default: {
//				std::cout << "Unhandled packet type!\n";
//				break;
//			}
//	}
//}
//bool State_Distribute::packet_distribute(sf::Packet &packet) {
//	if (! connection_verify()) {
//		return false;
//	}
//
//	if (_connection->host()) {
//		packet_handle(0, packet);
//	}
//	else {
//		_connection->send(packet);
//	}
//
//	return true;
//}
//void State_Distribute::update() {
//	if (! connection_verify()) {
//		return;
//	}
//
//	unsigned int client_id = 0;
//	sf::Packet packet;
//	if (_connection->receive(client_id, packet)) {
//		//HANDLE PACKET (SERVER HANDLES FOR ITSELF AFTER SENDING TO ALL, OTHERWISE CLIENTS JUST HANDLE HERE
//		packet_handle(client_id, packet);
//	}
//}
//Packet_Type State_Distribute::packet_type_read(sf::Packet &packet) {
//	//READS THE FIRST UINT8 FROM THE PACKET WHICH CAN TRANSLATE INTO A PACKET TYPE ENUM DEFINING THE TYPE OF PACKET
//	sf::Uint8 type_num = 0;
//	if (! (packet >> type_num)) {
//		return packet_type_none;
//	}
//	Packet_Type type = static_cast<Packet_Type>(type_num);
//	if ((type < Packet_Type_First) || (type > Packet_Type_Last)) {
//		return packet_type_none;
//	}
//
//	return type;
//}