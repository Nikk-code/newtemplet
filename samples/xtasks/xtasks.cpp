/*$TET$$header*/

#include <xtemplet.hpp>
#include <cmath>
#include <iostream>

class number : public templet::message {
public:
	number(templet::actor*a, templet::message_adaptor ma) :templet::message(a, ma) {}
	double num;
};

class sinetask : protected templet::base_task {
public:
	sinetask(templet::actor*a, templet::task_adaptor ta) :base_task(a, ta) {}
	void engine(templet::base_engine&te) { templet::base_task::engine(te); }
	void submit_arg(double n) { num = n; submit(); }
	double get_result() { return num; }
private:
	void run() override { (num = (num = sin(num))*num); }
	double num;
};
/*$TET$*/

#pragma templet !master(cw!number,sw!number)

struct master :public templet::actor {
	static void on_cw_adapter(templet::actor*a, templet::message*m) {
		((master*)a)->on_cw(*(number*)m);}
	static void on_sw_adapter(templet::actor*a, templet::message*m) {
		((master*)a)->on_sw(*(number*)m);}

	master(templet::engine&e) :master() {
		master::engines(e);
	}

	master() :templet::actor(true),
		cw(this, &on_cw_adapter),
		sw(this, &on_sw_adapter)
	{
/*$TET$master$master*/
		x = 0.0;
/*$TET$*/
	}

	void engines(templet::engine&e) {
		templet::actor::engine(e);
/*$TET$master$engines*/
/*$TET$*/
	}

	void start() {
/*$TET$master$start*/
		cw.num = sw.num = x;
		cw.send(); sw.send();
/*$TET$*/
	}

	inline void on_cw(number&m) {
/*$TET$master$cw*/
		sum_sin2x_and_cos2x();
/*$TET$*/
	}

	inline void on_sw(number&m) {
/*$TET$master$sw*/
		sum_sin2x_and_cos2x();
/*$TET$*/
	}

	number cw;
	number sw;

/*$TET$master$$footer*/
	void sum_sin2x_and_cos2x() {
		if (access(cw) && access(sw)) {
			sin2x_and_cos2x = sw.num + cw.num;
			stop();
		}
	}

	double x;
	double sin2x_and_cos2x;
/*$TET$*/
};

#pragma templet cworker(cw?number,t:base)

struct cworker :public templet::actor {
	static void on_cw_adapter(templet::actor*a, templet::message*m) {
		((cworker*)a)->on_cw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::task*t) {
		((cworker*)a)->on_t(*(templet::base_task*)t);}

	cworker(templet::engine&e,templet::base_engine&te_base) :cworker() {
		cworker::engines(e,te_base);
	}

	cworker() :templet::actor(false),
		t(this, &on_t_adapter)
	{
/*$TET$cworker$cworker*/
		_cw = 0;
/*$TET$*/
	}

	void engines(templet::engine&e,templet::base_engine&te_base) {
		templet::actor::engine(e);
		t.engine(te_base);
/*$TET$cworker$engines*/
/*$TET$*/
	}

	inline void on_cw(number&m) {
/*$TET$cworker$cw*/
		_cw = &m;
		t.submit();
/*$TET$*/
	}

	inline void on_t(templet::base_task&t) {
/*$TET$cworker$t*/
		double num;
		_cw->num = (num = cos(_cw->num))*num;
		_cw->send();
/*$TET$*/
	}

	void cw(number&m) { m.bind(this, &on_cw_adapter); }
	templet::base_task t;

/*$TET$cworker$$footer*/
	number* _cw;
/*$TET$*/
};

#pragma templet sworker(sw?number,t:base.sinetask)

struct sworker :public templet::actor {
	static void on_sw_adapter(templet::actor*a, templet::message*m) {
		((sworker*)a)->on_sw(*(number*)m);}
	static void on_t_adapter(templet::actor*a, templet::task*t) {
		((sworker*)a)->on_t(*(sinetask*)t);}

	sworker(templet::engine&e,templet::base_engine&te_base) :sworker() {
		sworker::engines(e,te_base);
	}

	sworker() :templet::actor(false),
		t(this, &on_t_adapter)
	{
/*$TET$sworker$sworker*/
		_sw = 0;
/*$TET$*/
	}

	void engines(templet::engine&e,templet::base_engine&te_base) {
		templet::actor::engine(e);
		t.engine(te_base);
/*$TET$sworker$engines*/
/*$TET$*/
	}

	inline void on_sw(number&m) {
/*$TET$sworker$sw*/
		_sw = &m;
		t.submit_arg(m.num);
/*$TET$*/
	}

	inline void on_t(sinetask&t) {
/*$TET$sworker$t*/
		_sw->num = t.get_result();
		_sw->send();
/*$TET$*/
	}

	void sw(number&m) { m.bind(this, &on_sw_adapter); }
	sinetask t;

/*$TET$sworker$$footer*/
	number* _sw;
/*$TET$*/
};

/*$TET$$footer*/

int main()
{
	templet::engine e;
	templet::base_engine te;

	master  a_master(e);
	cworker a_cos_worker(e, te);
	sworker a_sin_worker[1];

	a_sin_worker[0].engines(e, te);

	a_cos_worker.cw(a_master.cw);
	a_sin_worker[0].sw(a_master.sw);

	a_master.x = (double)rand();

	e.dispatch();
	te.run();

	if (e.graceful_shutdown()) {
		std::cout << "sin2(" << a_master.x << ") + cos2(" << a_master.x << ") = " << a_master.sin2x_and_cos2x << std::endl;
		return EXIT_SUCCESS;
	}

	std::cout << "something went wrong" << std::endl;
	return EXIT_FAILURE;
}
/*$TET$*/
