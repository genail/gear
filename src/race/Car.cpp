/*
 * Car.cpp
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#include "race/Car.h"
#include "Properties.h"
#include "Message.h"
#include "race/Level.h"
#include "race/RacePlayer.h"

#include <ClanLib/core.h>
#include <ClanLib/display.h>

Car::Car(RacePlayer *p_player) :
	m_player(p_player),
	m_level(NULL),
	m_sprite(),
	m_locked(false),
	m_position(0.0f, 0.0f), //najnowsza pozycja autka
	m_rotation(0, cl_degrees), // kąt pod jakim stoi autko
	m_turn(0.0f), //na boki
	m_acceleration(false), //do przodu
	m_brake(false), //do tyłu
	m_speed(0.0f),
	m_angle(0.0f),
	m_inputChecksum(0),
	m_lap(0)
{

}

Car::~Car() {
}

void Car::draw(CL_GraphicContext &p_gc) {

	// TODO: move to load();
	if (m_sprite.is_null()) {
		m_sprite = CL_Sprite(p_gc, "race/car", Stage::getResourceManager());
	}

	if (m_nickDisplayFont.is_null()) {
		CL_FontDescription fontDesc;

		fontDesc.set_typeface_name("resources/DejaVuSansCondensed-BoldOblique.ttf");
		fontDesc.set_height(12);

		m_nickDisplayFont = CL_Font_Freetype(p_gc, fontDesc);
	}

	p_gc.push_modelview();

	p_gc.mult_translate(m_position.x, m_position.y);

	// display nickname
	const CL_Size nameLabelSize = m_nickDisplayFont.get_text_size(p_gc, m_player->getPlayer().getName());
	m_nickDisplayFont.draw_text(p_gc, -nameLabelSize.width / 2, -20, m_player->getPlayer().getName());

	p_gc.mult_rotate(m_rotation, 0, 0, 1);

	m_sprite.draw(p_gc, 0, 0);
	
#ifndef NDEBUG
	if (Properties::getPropertyAsBool("debug.draw_vectors")) {
		debugDrawLine(p_gc, 0, 0, forceVector.x, forceVector.y, CL_Colorf::red);
		debugDrawLine(p_gc, 0, 0, driftVector.x*10, driftVector.y*10, CL_Colorf::green);
		debugDrawLine(p_gc, 0, 0, accelerationVector.x/10, accelerationVector.y/10, CL_Colorf::blue);
		debugDrawLine(p_gc, 0, 0, m_moveVector.x/10, m_moveVector.y/10, CL_Colorf::black);
	}
#endif // NDEBUG


	p_gc.pop_modelview();

#ifndef NDEBUG

	if (Properties::getPropertyAsBool("debug.draw_checkpoints")) {

		// draw checkpoints
		const CL_Vec4f red_color(1.0f, 0.0f, 0.0f, 0.3f);
		const CL_Vec4f green_color(0.0f, 1.0f, 0.0f, 0.3f);

		for (std::vector<Checkpoint>::iterator itor = m_checkpoints.begin(); itor != m_checkpoints.end(); ++itor) {
			const CL_Rectf &rect = itor->getRect();
			const CL_Vec4f color = itor->isPassed() ? green_color : red_color;

	//		CL_Console::write_line(CL_String8("rect: left=") + CL_StringHelp::float_to_local8(rect.left) + CL_String8(", top=") + CL_StringHelp::float_to_local8(rect.top));

			CL_Vec2f positions[] = {
					CL_Vec2f(rect.left, rect.top),
					CL_Vec2f(rect.left, rect.bottom),
					CL_Vec2f(rect.right, rect.bottom),
					CL_Vec2f(rect.right, rect.top)
			};

			CL_Vec4f colors[] = { color, color, color, color };

			CL_PrimitivesArray vertices(p_gc);
			vertices.set_attributes(0, positions);
			vertices.set_attributes(1, colors);

			p_gc.set_program_object(cl_program_color_only);
			p_gc.draw_primitives(cl_quads, 4, vertices);
		}

	}
#endif // NDEBUG
}

#ifndef NDEBUG
void Car::debugDrawLine(CL_GraphicContext &p_gc, float x1, float y1, float x2, float y2, const CL_Color& p_color) {

	CL_Vec4f color(p_color.get_red_f(), p_color.get_green_f(), p_color.get_blue_f(), p_color.get_alpha_f());

	CL_Vec2f positions[] = {
			CL_Vec2f(x1, y1),
			CL_Vec2f(x2, y2),
	};

	CL_Vec4f colors[] = { color, color };

	CL_PrimitivesArray vertices(p_gc);
	vertices.set_attributes(0, positions);
	vertices.set_attributes(1, colors);

	p_gc.set_program_object(cl_program_color_only);
	p_gc.draw_primitives(cl_lines, 2, vertices);
}
#endif // NDEBUG

void Car::load(CL_GraphicContext &p_gc) {


}

void Car::update(unsigned int elapsedTime) {

	// don't do anything if car is locked
	if (m_locked) {
		return;
	}

	static const float BRAKE_POWER = 400.0f;

	static const float ACCEL_SPEED = 300.0f;
	static const float MAX_SPEED = 500.0f;

	static const float AIR_RESIST = 0.2f;
	
	static const float MAX_ANGLE = 60.0f;

	const float delta = elapsedTime / 1000.0f;

	// calculate input checksum and if its different than last one, then
	// invoke the signal
	const int inputChecksum = calculateInputChecksum();

	if (inputChecksum != m_inputChecksum) {
		m_statusChangeSignal.invoke(*this);
		m_inputChecksum = inputChecksum;
	}

	// mark checkpoints
	for (std::vector<Checkpoint>::iterator itor = m_checkpoints.begin(); itor != m_checkpoints.end(); ++itor) {

		if (itor->isPassed()) {
			continue;
		}

		if (itor->getRect().contains(m_position)) {
			itor->setPassed(true);
		}
	}

	// check lap progress
	if (areAllCheckpointsPassed()) {
		// check for last lap checkpoint
		if (m_lapCheckpoint.getRect().contains(m_position)) {
			// got lap
			++m_lap;
			resetCheckpoints();
		}
	}

	//turning	
	if( m_turn == -1.0f )
		m_angle = -MAX_ANGLE;
	else if( m_turn == 1.0f )
		m_angle = MAX_ANGLE;
	else 
		m_angle = 0.0f;

	// acceleration

	if (m_acceleration) {
		const float speedChange = ACCEL_SPEED * delta;

		m_speed += speedChange;

		if (m_speed > MAX_SPEED) {
			m_speed = MAX_SPEED;
		}

	}

	// brake
	if (m_brake) {
		const float speedChange = BRAKE_POWER * delta;

		m_speed -= speedChange;

		if (m_speed < -MAX_SPEED / 2) {
			m_speed = -MAX_SPEED / 2;
		}
	}

	// air resistance
	m_speed -= delta * AIR_RESIST * m_speed;
	
	// ground resistance
	if (m_level != NULL) {
		const float groundResist = m_level->getResistance(m_position.x, m_position.y);
		m_speed -= delta * groundResist * m_speed;
	}

	// rotation
		
	const float rad = m_rotation.to_radians();

	accelerationVector.x = cos(rad);
	accelerationVector.y = sin(rad);

	accelerationVector.normalize();
	accelerationVector *= m_speed;
	
	if( m_angle < 0.0f ) {
 		forceVector.x = sin(rad);
		forceVector.y = -cos(rad);
	
		forceVector.normalize();
		
		forceVector *= 0.1f * m_speed/7.0f;	
	}
	else if( m_angle > 0.0f ){
		forceVector.x = -sin(rad);
		forceVector.y = cos(rad);
	
		forceVector.normalize();
		
		forceVector *= 0.1f * m_speed/7.0f;
	}
	
	Message::out() << "wartość: " << (tan(60.0f)) << std::endl;
	
	
	if( m_angle != 0.0f )
		m_moveVector = accelerationVector + forceVector;
	else 
		m_moveVector = accelerationVector;
	
	
	/*const float rad = m_rotation.to_radians();

	m_moveVector.x = cos(rad);
	m_moveVector.y = sin(rad);

	m_moveVector.normalize();
	m_moveVector *= m_speed;
	*/
	// movement
	if( m_angle != 0.0f )
		m_rotation.set_degrees( atan2( m_moveVector.y, m_moveVector.x ) * 180.0f / 3.14f );
		
	//m_rotation.normalize();

	CL_Vec2f currentMoveVector = m_moveVector * delta;
	m_position.x += currentMoveVector.x;
	m_position.y += currentMoveVector.y;
	
#ifndef NDEBUG
			Stage::getDebugLayer()->putMessage(CL_String8("force"),  CL_StringHelp::float_to_local8(forceVector.length()));
			Stage::getDebugLayer()->putMessage(CL_String8("speed"),  CL_StringHelp::float_to_local8(m_speed));
			if (m_level != NULL) {
				Stage::getDebugLayer()->putMessage(CL_String8("resist"), CL_StringHelp::float_to_local8(m_level->getResistance(m_position.x, m_position.y)));
			}
#endif
}

int Car::prepareStatusEvent(CL_NetGameEvent &p_event) {
	CL_NetGameEventValue posX(m_position.x);
	CL_NetGameEventValue posY(m_position.y);
	CL_NetGameEventValue rotation(m_rotation.to_degrees());
	CL_NetGameEventValue turn(m_turn);
	CL_NetGameEventValue accel(m_acceleration);
	CL_NetGameEventValue brake(m_brake);
	CL_NetGameEventValue moveX(m_moveVector.x);
	CL_NetGameEventValue moveY(m_moveVector.y);
	CL_NetGameEventValue speed(m_speed);

	int c = 0;

	p_event.add_argument(posX);     ++c;
	p_event.add_argument(posY);     ++c;
	p_event.add_argument(rotation); ++c;
	p_event.add_argument(turn);     ++c;
	p_event.add_argument(accel);    ++c;
	p_event.add_argument(brake);    ++c;
	p_event.add_argument(moveX);    ++c;
	p_event.add_argument(moveY);    ++c;
	p_event.add_argument(speed);    ++c;

	return c;
}

int Car::applyStatusEvent(const CL_NetGameEvent &p_event, int p_beginIndex) {
	int i = p_beginIndex;

	m_position.x =   (float) p_event.get_argument(i++);
	m_position.y =   (float) p_event.get_argument(i++);
	m_rotation =     CL_Angle((float) p_event.get_argument(i++), cl_degrees);
	m_turn =         (float) p_event.get_argument(i++);
	m_acceleration =  (bool) p_event.get_argument(i++);
	m_brake =         (bool) p_event.get_argument(i++);
	m_moveVector.x = (float) p_event.get_argument(i++);
	m_moveVector.y = (float) p_event.get_argument(i++);
	m_speed =        (float) p_event.get_argument(i++);

	return i;
}

int Car::calculateInputChecksum() const {
	int checksum = 0;
	checksum |= (int) m_acceleration;
	checksum |= ((int) m_brake) << 1;
	checksum += m_turn * 10000.0f;

	return checksum;
}

bool Car::areAllCheckpointsPassed() const {
	for (std::vector<Checkpoint>::const_iterator itor = m_checkpoints.begin(); itor != m_checkpoints.end(); ++itor) {
		if (!itor->isPassed()) {
			return false;
		}
	}

	return true;
}

bool Car::resetCheckpoints() {
	for (std::vector<Checkpoint>::iterator itor = m_checkpoints.begin(); itor != m_checkpoints.end(); ++itor) {
		itor->setPassed(false);
	}
}

void Car::setStartPosition(int p_startPosition) {
	m_position = m_level->getStartPosition(p_startPosition);

	// stop the car!
	m_rotation = CL_Angle::from_degrees(-90);
	m_turn = 0;
	m_acceleration = false;
	m_brake = false;
	m_moveVector = CL_Vec2f();
	accelerationVector = CL_Vec2f();
	forceVector = CL_Vec2f();
	driftVector = CL_Vec2f();
	m_speed = 0.0f;
	m_angle = 0.0f;
	m_lap = 1;

	// send the status change to other players
	m_statusChangeSignal.invoke(*this);
}
