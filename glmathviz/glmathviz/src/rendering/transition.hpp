#ifndef TRANSITION_H
#define TRANSITION_H

template <typename T>
class Transition {
private:
	T cur;
	double cur_t;
	double duration;
	bool running;
	bool cyclical;

protected:
	T start;
	T end;
	
	// function to be overridden in subclasses
	virtual T calculateNew(double t) { return end; }

public:
	Transition(T start, T end, double duration)
		: start(start), end(end), cur(start),
		duration(duration), cur_t(0.0), 
		running(false) { }

	void update(double dt) {
		if (running) {
			cur_t += dt / duration;
			if (cur_t <= 0.0) {
				cur = start;
			}
			else if (cur_t >= 1.0) {
				cur = end;
				if (cyclical) {
					// cycle around
					cur_t -= (double)(int)cur_t;
					update(0.0);
				}
			}
			else {
				// calculate new transition point
				cur = calculateNew(cur_t);
			}
		}
	}

	double getDuration() {
		return duration;
	}

	void toggleRunning() {
		running = !running;
	}

	bool isRunning() {
		return running;
	}

	void setCyclical(bool cyclical = true) {
		this->cyclical = cyclical;
	}

	T getCurrent() {
		return cur;
	}
};

template <typename T>
class ProportionalTransition : public Transition<T> {
	T calculateNew(double t) {
		float prop = (float)calculateProportion(t);
		return (1.0f - prop) * this->start + prop * this->end;
	}

protected:
	// function to be overridden in subclasses
	virtual double calculateProportion(double t) { return 1.0; }

public:
	ProportionalTransition(T start, T end, double duration)
		: Transition<T>(start, end, duration) { }
};

template <typename T>
class LinearTransition : public ProportionalTransition<T> {
	double calculateProportion(double t) { return t; }

public:
	LinearTransition(T start, T end, double duration)
		: ProportionalTransition<T>(start, end, duration) { }
};

template <typename T>
class QuadraticTransition : public ProportionalTransition<T> {
	double calculateProportion(double t) { return t * t; }

public:
	QuadraticTransition(T start, T end, double duration)
		: ProportionalTransition<T>(start, end, duration) { }
};


template <typename T>
class StepTransition : public ProportionalTransition<T> {
	unsigned int noSteps;

	double calculateProportion(double t) {
		return floor(t * (double)noSteps) / (double)noSteps;
	}

public:
	StepTransition(T start, T end, double duration, unsigned int noSteps)
		: ProportionalTransition<T>(start, end, duration) {
		this->noSteps = noSteps > 0 ? noSteps : 1;
	}
};

typedef double(*transition_func)(double t);

template <typename T>
class CustomProportionalTransition : public ProportionalTransition<T> {
	transition_func func;

	double calculateProportion(double t) {
		return func(t);
	}

public:
	CustomProportionalTransition(T start, T end, double duration, transition_func func)
		: ProportionalTransition<T>(start, end, duration), func(func) { }
};

template<typename T>
class CubicBezierTransition : public ProportionalTransition<T> {
	glm::f64vec2 P0, P1, P2, P3;

	double calculateProportion(double t) {
		double t1 = 1 - t;

		glm::f64vec2 val = t1 * t1 * t1 * P0 // (1 - t)^3 * P0
			+ 3 * t1 * t1 * t * P1 // 3 (1 - t)^2 t P1
			+ 3 * t1 * t * t * P2 // 3 (1 - t) t^2 P2
			+ t * t * t * P3; // t^3 P3

		return val.y;
	}

public:
	CubicBezierTransition(T start,
		double t1, double p1,
		double t2, double p2,
		T end, double duration)
		: ProportionalTransition<T>(start, end, duration),
		P0(0.0, 0.0), P1(t1, p1), P2(t2, p2), P3(1.0, 1.0) { }

	static CubicBezierTransition<T> newEaseTransition(T start, T end, double duration) {
		return CubicBezierTransition<T>(start, 0.25, 0.1, 0.25, 1.0, end, duration);
	}
};

template <typename T>
class CubicBezierPath : public Transition<T> {
	T P0, P1, P2, P3;

	T calculateNew(double t) {
		double t1 = 1 - t;

		return (float)(t1 * t1 * t1) * P0 // (1 - t)^3 * P0
			+ 3 * (float)(t1 * t1 * t) * P1 // 3 (1 - t)^2 t P1
			+ 3 * (float)(t1 * t * t) * P2 // 3 (1 - t) t^2 P2
			+ (float)(t * t * t) * P3; // t^3 P3
	}

public:
	CubicBezierPath(T start, T P1, T P2, T end, double duration)
		: Transition<T>(start, end, duration),
		P0(start), P1(P1), P2(P2), P3(end) { }
};

typedef glm::vec3(*path_func)(double t);
class ParametrizedPath : public Transition<glm::vec3> {
	path_func func;

	double t0;
	double t1;

	glm::vec3 calculateNew(double t) {
		// LERP between t0 and t1
		t = t0 + t * (t1 - t0);
		return func(t);
	}

public:
	ParametrizedPath(path_func func, double t0, double t1, double duration)
		: Transition<glm::vec3>(func(t0), func(t1), duration),
		t0(t0), t1(t1), func(func) {}
};

#endif // TRANSITION_H