#ifndef INCLUDE_PLUGINGUI_H
#define INCLUDE_PLUGINGUI_H

#include <QWidget>

class Message;

class PluginGUI : public QWidget {
	Q_OBJECT

public:
	PluginGUI(QWidget* parent = NULL);
	virtual void destroy() = 0;

	virtual void setWidgetName(const QString& name);

	virtual void resetToDefaults() = 0;

	virtual QByteArray serializeGeneral() const;
	virtual bool deserializeGeneral(const QByteArray& data);
	virtual quint64 getCenterFrequency() const;

	virtual QByteArray serialize() const = 0;
	virtual bool deserialize(const QByteArray& data) = 0;

	virtual bool handleMessage(Message* message) = 0;
};

#endif // INCLUDE_PLUGINGUI_H