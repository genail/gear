/*
 * Copyright (c) 2009, Piotr Korzuszek, Paweł Rybarczyk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "race/Car.h"
#include "Properties.h"
#include "race/Level.h"
#include "Game.h"

#include <ClanLib/core.h>

/* Car width in pixels */
const int CAR_WIDTH = 18;

/* Car height in pixels */
const int CAR_HEIGHT = 24;

Car::Car() :
#ifdef CLIENT
	m_sprite(),
#endif // CLIENT
	m_level(NULL),
	m_locked(false),
	m_position(300.0f, 300.0f),
	m_rotation(0, cl_degrees),
	m_turn(0.0f),
	m_acceleration(false),
	m_brake(false),
	m_speed(0.0f),
	m_angle(0.0f),
	m_inputChecksum(0),
	m_lap(0),
	m_handbrake(false)
{
#ifndef SERVER
	// build car contour for collision check
	CL_Contour contour;

	const int halfWidth = CAR_WIDTH / 2;
	const int halfHeight = CAR_HEIGHT / 2;
	contour.get_points().push_back(CL_Pointf(-halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, halfHeight));
	contour.get_points().push_back(CL_Pointf(halfWidth, -halfHeight));
	contour.get_points().push_back(CL_Pointf(-halfWidth, -halfHeight));

	m_collisionOutline.get_contours().push_back(contour);

	m_collisionOutline.set_inside_test(true);

	m_collisionOutline.calculate_radius();
	m_collisionOutline.calculate_smallest_enclosing_discs();
#endif // !SERVERs
}

Car::~Car() {
}

#ifdef CLIENT
void Car::draw(CL_GraphicContext &p_gc) {
	
	//debugDrawLine(p_gc, m_position.x, m_position.y, m_position.x + m_moveVector.x/2, m_position.y + m_moveVector.y/2, CL_Colorf::red);
	//debugDrawLine(p_gc, m_position.x, m_position.y, m_position.x + accelerationVector.x/2, m_position.y + accelerationVector.y/2, CL_Colorf::black);
	
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
	const CL_String &playerName = Game::getInstance().getPlayer().getName();
	const CL_Size nameLabelSize = m_nickDisplayFont.get_text_size(p_gc, playerName);
	m_nickDisplayFont.draw_text(p_gc, -nameLabelSize.width / 2, -20, playerName);

	p_gc.mult_rotate(m_rotation, 0, 0, 1);

	m_sprite.draw(p_gc, 0, 0);
	
#ifndef NDEBUG
	if (Properties::getPropertyAsBool("debug.draw_vectors")) {
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

#endif // CLIENT

void Car::update(unsigned int elapsedTime) {
	
	// don't do anything if car is locked
	if (m_locked) {
		return;
	}
	
	static const float BRAKE_POWER = 100.0f;
	
	static const float ACCEL_SPEED = 200.0f;
	static const float MAX_SPEED = 500.0f;
	
	static const float AIR_RESIST = 0.2f;
	
	static const float MAX_ANGLE = 50.0f;
	
	static const float MAX_TENACITY = 0.2f;
	static const float MIN_TENACITY = 0.05f;
	
	
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
	const float angle = MAX_ANGLE * m_turn;
	
	// acceleration speed
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
	const float rad = m_rotation.to_radians(); // kąt autka w radianach
	
	// wektor zmiany kierunku
	CL_Vec2f changeVector;
	
	if( angle < 0.0f ) { // skręt w jedną stronę
		// przygotowanie wektora skrętu (zmiany kierunku jazdy)
 		changeVector.x = sin(rad);
		changeVector.y = -cos(rad);
		changeVector.normalize();
		changeVector *= tan( -angle ) * fabs(m_speed) / 7.0f; // modyfikacja siły skrętu
	}
	else if( angle > 0.0f ){ // skręt w drugą stronę, wszystko analogicznie
		changeVector.x = -sin(rad);
		changeVector.y = cos(rad);
		changeVector.normalize();
		changeVector *= tan( angle ) * fabs(m_speed) / 7.0f;
	}
	
	// wektor przyspieszenia bez uwzględnionej zmiany kierunku
		accelerationVector.x = cos(rad);
		accelerationVector.y = sin(rad);
		
		accelerationVector.normalize();
		accelerationVector *= m_speed;
		// wektor prędkości, zgodnie z którym pojechałoby autko bez poślizgu
		if( angle != 0.0f ) // sumowanie wektorow: skrętu i prostej jazdy
			accelerationVector = changeVector + accelerationVector;
	
	// calculates current tenacity
	const float max_tratio = MAX_TENACITY / MIN_TENACITY;
	
	float tratio = (max_tratio / MAX_SPEED) * fabs(m_speed);
	if (fabs(m_speed) == 0)
		tratio = 1;
	
	const float tenacity = MAX_TENACITY / tratio;
	
	// wektor o który zostanie przesunięte autko (już z poślizgiem)
	CL_Vec2f realVector = m_moveVector + ( accelerationVector * tenacity );
	realVector.normalize();
	realVector *= fabs(m_speed);
	m_moveVector = realVector;
	
	// update position
	m_position.x += m_moveVector.x * delta;
	m_position.y += m_moveVector.y * delta;
	
	// update rotation of car when changing direction
	if( m_turn != 0.0f ) { // wykonywany jest skręt
		
		// tworzymy sobie osobne wektory dla rotacji
		CL_Vec2f rotVector; // wektor rotacji
		CL_Vec2f changeRotVector; // wektor zmiany rotacji (w danej chwili)
		
		rotVector.x = cos(rad);
		rotVector.y = sin(rad);
		rotVector.normalize();
		
		if( m_turn < 0.0f ) { // skręt w jedną stronę
			changeRotVector.x = sin(rad);
			changeRotVector.y = -cos(rad);
			changeRotVector.normalize();
			changeRotVector *= 2.4f * delta;
		}
		else if ( m_turn > 0.0f ) { // skręt w drugą stronę
			changeRotVector.x = -sin(rad);
			changeRotVector.y = cos(rad);
			changeRotVector.normalize();
			changeRotVector *= 2.4f * delta;
		}
		
		rotVector += changeRotVector;
		m_rotation.set_degrees( atan2( rotVector.y, rotVector.x ) * 180.0f / 3.14f );
	}
	
#ifdef CLIENT
#ifndef NDEBUG
			Stage::getDebugLayer()->putMessage(CL_String8("speed"),  CL_StringHelp::float_to_local8(m_speed));
			if (m_level != NULL) {
				Stage::getDebugLayer()->putMessage(CL_String8("resist"), CL_StringHelp::float_to_local8(m_level->getResistance(m_position.x, m_position.y)));
			}
#endif // NDEBUG
#endif // CLIENT
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
	CL_NetGameEventValue lap(m_lap);

	int c = 0;

	p_event.add_argument(posX);		++c;
	p_event.add_argument(posY);		++c;
	p_event.add_argument(rotation);	++c;
	p_event.add_argument(turn);		++c;
	p_event.add_argument(accel);	++c;
	p_event.add_argument(brake);	++c;
	p_event.add_argument(moveX);	++c;
	p_event.add_argument(moveY);	++c;
	p_event.add_argument(speed);	++c;
	p_event.add_argument(lap);		++c;

	return c;
}

Net::CarState Car::prepareCarState()
{
	Net::CarState state;

	state.setPosition(m_position);
	state.setRotation(m_rotation);
	state.setMovement(m_moveVector);
	state.setSpeed(m_speed);
	state.setTurn(m_turn);

	if (m_acceleration && !m_brake) {
		state.setAcceleration(1.0f);
	} else if (!m_acceleration && m_brake) {
		state.setAcceleration(-1.0f);
	} else {
		state.setAcceleration(0.0f);
	}

	return state;
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
	m_lap =            (int) p_event.get_argument(i++);

	return i;
}

void Car::applyCarState(const Net::CarState &p_carState)
{
	m_position = p_carState.getPosition();
	m_rotation = p_carState.getRotation();
	m_moveVector = p_carState.getMovement();
	m_speed = p_carState.getSpeed();
	m_turn = p_carState.getTurn();
	m_acceleration = p_carState.getAcceleration() > 0.0f;
	m_brake = p_carState.getAcceleration() < 0.0f;
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

void Car::resetCheckpoints() {
	for (std::vector<Checkpoint>::iterator itor = m_checkpoints.begin(); itor != m_checkpoints.end(); ++itor) {
		itor->setPassed(false);
	}
}

void Car::setStartPosition(int p_startPosition) {
	if (m_level != NULL) {
		m_position = m_level->getStartPosition(p_startPosition);
	} else {
		cl_log_event("warning", "Car not on Level.");
		m_position = CL_Pointf(300, 300);
	}

	// stop the car!
	m_rotation = CL_Angle::from_degrees(-90);
	m_turn = 0;
	m_acceleration = false;
	m_brake = false;
	m_moveVector = CL_Vec2f();
	accelerationVector = CL_Vec2f();
	m_speed = 0.0f;
	m_angle = 0.0f;
	m_lap = 1;

	// send the status change to other players
	m_statusChangeSignal.invoke(*this);
}

bool Car::isDrifting() const {
	
	static const float MIN_SPEED = 320.0f;
	static const float MIN_TURN = 0.5f;
	
	if (m_brake && m_speed >= MIN_SPEED) return true;
	else if (fabs(m_turn) >= MIN_TURN && m_speed >= MIN_SPEED) return true;
	else return false;
	
}

#ifdef CLIENT
CL_CollisionOutline Car::calculateCurrentCollisionOutline() const
{
	CL_CollisionOutline outline(m_collisionOutline);

//	outline.calculate_smallest_enclosing_discs();
//	outline.set_inside_test(true);


	// transform the outline
	CL_Angle angle(90, cl_degrees);
	angle += m_rotation;

	outline.set_angle(angle);
	outline.set_translation(m_position.x, m_position.y);

	return outline;
}
#endif
