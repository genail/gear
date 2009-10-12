/*
 * Car.h
 *
 *  Created on: 2009-09-08
 *      Author: chudy
 */

#ifndef CAR_H_
#define CAR_H_

#include "race/Checkpoint.h"

#include <ClanLib/core.h>
#include <ClanLib/network.h>

class Level;
class RacePlayer;

#ifdef CLIENT // client-only code
#include "graphics/Stage.h"
#include "graphics/Drawable.h"

#define CLASS_CAR class Car : public Drawable


#else // server-only code

#define CLASS_CAR class Car

#endif // CLIENT

CLASS_CAR
{

	public:
		Car(RacePlayer* p_player);
		virtual ~Car();

		int getLap() const { return m_lap; }

		RacePlayer* getPlayer() const { return m_player; }

		const CL_Pointf& getPosition() const { return m_position; }

		float getRotation() const { return m_rotation.to_degrees(); }
		
		float getRotationRad() const { return m_rotation.to_radians(); }

		float getSpeed() const { return m_speed; }

		/** @return Car speed in km/s */
		float getSpeedKMS() const { return m_speed / 3.0f; }

		bool isDrifting() const;

		int prepareStatusEvent(CL_NetGameEvent &p_event);

		int applyStatusEvent(const CL_NetGameEvent &p_event, int p_beginIndex = 0);

		void setAcceleration(bool p_value) {
			m_acceleration = p_value;
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("accel"),  CL_StringHelp::bool_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

		void setBrake(bool p_value) {
			m_brake = p_value;
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("brake"),  CL_StringHelp::bool_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

		void setLap(int p_lap) { m_lap = p_lap; }

		/**
		 * Sets if car movement should be locked (car won't move).
		 */
		void setLocked(bool p_locked);

		void setTurn(float p_value) {
			m_turn = normalize(p_value);
#ifndef NDEBUG
#ifdef CLIENT
			Stage::getDebugLayer()->putMessage(CL_String8("turn"),  CL_StringHelp::float_to_local8(p_value));
#endif // CLIENT
#endif // !NDEBUG
		}

		void setPosition(const CL_Pointf &p_position) { m_position = p_position; }

		void setRotation(float p_rotation) { m_rotation.set_degrees(p_rotation); }
		
		void setHandbrake(bool p_handbrake) { m_handbrake = p_handbrake; }

		/**
		 * Sets the car position at selected <code>p_startPosition</code>
		 * which is a number >= 1.
		 *
		 * @param p_startPosition Car start position.
		 */
		void setStartPosition(int p_startPosition);

		void update(unsigned int elapsedTime);

		CL_Signal_v1<Car &> &sigStatusChange() { return m_statusChangeSignal; }

#ifdef CLIENT
		virtual void draw(CL_GraphicContext &p_gc);

		virtual void load(CL_GraphicContext &p_gc);
#endif // CLIENT

	private:
		
		/** Parent player */
		RacePlayer* m_player;

		/** Parent level */
		Level* m_level;

		/** Locked state. If true then car shoudn't move. */
		bool m_locked;

		/** Central position on map */
		CL_Pointf m_position;

		/** Rotation in degrees CCW from positive X axis */
		CL_Angle m_rotation;

		/** Current turn. -1 is maximum left, 0 is center and 1 is maximum right */
		float m_turn;

		/** Acceleration switch */
		bool m_acceleration;

		/** Brake switch */
		bool m_brake;
		
		/** Handbrake switch */
		bool m_handbrake;

		/** Move vector */
		CL_Vec2f m_moveVector;
		CL_Vec2f accelerationVector;
		CL_Vec2f forceVector;
		CL_Vec2f driftVector;

		/** Current speed */
		float m_speed;
		
		/** angle */
		float m_angle;

		/** Input state changed signal */
		CL_Signal_v1<Car &> m_statusChangeSignal;

		/** Input checksum */
		int m_inputChecksum;

		/** Lap number */
		int m_lap;

		/** Level checkpoints and pass state. Filled in by parent Level */
		std::vector<Checkpoint> m_checkpoints;

		/** Final lap checkpoint. Filled by parent Level */
		Checkpoint m_lapCheckpoint;

		int calculateInputChecksum() const;

		float normalize(float p_value) const;

		bool areAllCheckpointsPassed() const;

		bool resetCheckpoints();

		friend class Level;

#ifdef CLIENT
		/** Car sprite */
		CL_Sprite m_sprite;

		/** Nickname display font */
		CL_Font_Freetype m_nickDisplayFont;

#ifndef NDEBUG
		void debugDrawLine(CL_GraphicContext &p_gc, float x1, float y1, float x2, float y2, const CL_Color& p_color);
#endif // !NDEBUG
#endif // CLIENT


};

inline float Car::normalize(float p_value) const {
	if (p_value < -1.0f) {
		p_value = 1.0f;
	} else if (p_value > 1.0f) {
		p_value = 1.0f;
	}

	return p_value;
}

#endif /* CAR_H_ */
