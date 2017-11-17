#pragma once



class AParticle // represents an individual particle
{
	Point previous_position; // previous position
	Point current_position; // current position
	Vec3 direction; // vector that represents the 'force' needed to change the particle position for a specific interval
	Vec3 gravity; // vector representing the force of gravity
	int lifetime; // how long this particle will last in milliseconds
	int lifeleft; // how much life is left in the particle
	float distance_travelled; // how far has this particle travelled?

	float orig_r; float orig_g; float orig_b; float orig_a;
	float final_r; float final_g; float final_b; float final_a;
	float current_r; float current_g; float current_b; float current_a;

	//	private Color cur_color; // current color of particle
	//	private Color last_color; // previous color of particle
	//	private Color delta_color; // change of color
	//	private Point prev; //  previous position

public:
	int get_lifeleft() { return lifeleft; }

	float getr() { return current_r; }
	float getg() { return current_g; }
	float getb() { return current_b; }
	float geta() { return current_a; }

	bool expired() {
		if (lifeleft <= 0)
			return true;
		else
			return false;
	}

	void subtract_lifetime(int s) { // subtract s from value in lifetime
		if (lifeleft - s < 0) {
			lifeleft = 0;
		}
		else {
			lifeleft -= s;
		}
	}

	AParticle() {
		current_position.putx(0);
		current_position.puty(0);
		current_position.putz(0);
		lifetime = 0;
		distance_travelled = 0.0f;
		orig_r = orig_g = orig_b = orig_a = 0.0f;
	}

	// regarding color, transition from r,g,b,a to final_r,final_g,final_b,final_a during the life of the particle
	AParticle(float ptx, float pty, float ptz, // initial position
		float dira, float dirb, float dirc, // velocity and direction vector
		float r, float g, float b, float a, // initial color
		float f_r, float f_g, float f_b, float f_a, // final color
		Vec3 gr, // gravity vector
		int iLifetime) // lifetime in milliseconds
	{
		current_position.putx(ptx);
		current_position.puty(pty);
		current_position.putz(ptz);

		current_position.putcolor(r, g, b, a);

		// save original and final color for reference when calculating color changes over the life of the particle
		current_r = orig_r = r;		current_g = orig_g = g;   current_b = orig_b = b;   current_a = orig_a = a;
		final_r = f_r; final_g = f_g; final_b = f_b; final_a = f_a;

		direction.seta(dira);
		direction.setb(dirb);
		direction.setc(dirc);

		gravity = gr;
		lifetime = lifeleft = iLifetime;
		distance_travelled = 0.0f;
	}

	// change the position of the particle
	void UpdatePosition() {
		previous_position.putx(current_position.getx());
		previous_position.puty(current_position.gety());
		previous_position.putz(current_position.getz());

		current_position.add_vector(direction);
	}

	void applyGravity() {
		// calculate life elapsed in ms
		int lifeElapsed = lifetime - lifeleft;
		// Magnify the gravity vector based on the amount of time gravity has been acting on it
		Vec3 acceleration = gravity.scale(lifeElapsed);
		current_position.add_vector(acceleration);
	}

	void UpdateAttributes() {
		float pct_life = (float)lifeleft / (float)lifetime;

		float color_range_r = final_r - orig_r;
		float color_range_g = final_g - orig_g;
		float color_range_b = final_b - orig_b;
		float color_range_a = final_a - orig_a;

		// change only the b color component for now
		current_r = orig_r + (1 - pct_life) * color_range_r;
		current_g = orig_g + (1 - pct_life) * color_range_g;
		current_b = orig_b + (1 - pct_life) * color_range_b;
		current_a = orig_a + (1 - pct_life) * color_range_a;
	}

	void UpdateDistance() {
		distance_travelled += previous_position.get_distance(current_position);
	}

	// get the point to render after it has been influenced by relevant parameters
	Point getRenderPos() {
		//Vec3 v = new Vec3(previous_position, current_position); // direction the particle is currently traveling
		//float displacement = FloatMath.sin(distance_travelled);
		//TODO: Return the current position with a random height (y) displacement added
		return current_position;
	}

	Point getPreviousPos() {
		return previous_position;
	}
};

