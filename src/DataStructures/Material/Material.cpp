#include "Material.h"

Material::Material()
{
	this->ka = CRAB::Vector4Df{ 0.1f, 0.1f, 0.1f, 0 };
	this->kd = CRAB::Vector4Df{ 1.0f, 1.0f, 1.0f, 0 };
	this->ks = CRAB::Vector4Df{ 1.0f, 1.0f, 1.0f, 0 };
	this->alfa = 100;
}

Material::Material(CRAB::Vector4Df ka, CRAB::Vector4Df kd, CRAB::Vector4Df ks, float alfa)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->alfa = alfa;
}

Material::~Material()
{

}