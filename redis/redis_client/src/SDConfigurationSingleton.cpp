#include "SDConfigurationSingleton.h"

using namespace std;

SDConfigurationSingleton* SDConfigurationSingleton::m_instance = new SDConfigurationSingleton();

void SDConfigurationSingleton::init(SDConfiguration* config)
{
	m_config = config;
}

SDConfiguration* SDConfigurationSingleton::get_config()
{
	return (m_config);
}

