#include "stdafx.h"

Injector::Injector() {

}
Injector::~Injector() {

}

void Injector::inject() {
	do_inject();
}

void Injector::free() {
	do_free();
}