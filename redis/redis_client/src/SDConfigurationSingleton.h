#ifndef SD_CONFIGURATION_SINGLETON_H
#define SD_CONFIGURATION_SINGLETON_H

#include <common/SDConfiguration.h>


class SDConfigurationSingleton
{
public:
	~SDConfigurationSingleton(){delete m_config;}

	static SDConfigurationSingleton* get_instance(){return m_instance;}

	void init(SDConfiguration* config);
	SDConfiguration* get_config();

protected:
	SDConfigurationSingleton(){m_config=NULL;}
	SDConfigurationSingleton(const SDConfigurationSingleton&){}
	SDConfigurationSingleton& operator=(const SDConfigurationSingleton&){return *this;}

private:
	static SDConfigurationSingleton* m_instance;

	SDConfiguration* m_config;
};

#endif /* end SD_CONFIGURATION_SINGLETON_H */

