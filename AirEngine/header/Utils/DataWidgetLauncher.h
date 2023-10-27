#pragma once
#include <qobject.h>
#include <qwidget.h>
#include <qevent.h>

namespace AirEngine
{
	namespace Utils
	{
		template<class TData>
		class DataWidgetLauncher : public QObject
		{
		public:
			class DataWidget final: public QWidget
			{
			private:
				bool& _isClosed;
				void closeEvent(QCloseEvent* event)
				{
					_isClosed = true;
				}
			public:
				DataWidget(bool& isClosed)
					: _isClosed(isClosed)
				{
					setAttribute(Qt::WA_DeleteOnClose);
				}
			};
		protected:
			TData& _data;
			DataWidget* _widget;
			bool _isClosed;
			virtual void OnSetUp()
			{

			}
		private:
			virtual bool event(QEvent* ev)
			{
				if (ev->type() == QEvent::User)
				{
					_widget = new DataWidget(_isClosed);
					OnSetUp();
					_widget->show();
					return true;
				}
				return QObject::event(ev);
			}
		public:
			DataWidgetLauncher(TData& data)
				: _data(data)
				, _widget(nullptr)
				, _isClosed(false)
			{

			}
			~DataWidgetLauncher()
			{
				if (!_isClosed)
				{
					_widget->close();
				}
			}
			void Launch()
			{
				moveToThread(QApplication::instance()->thread());
				QApplication::postEvent(this, new QEvent(QEvent::User));
			}
			void Terminate()
			{
				deleteLater();
			}
			TData& Data()
			{
				return _data;
			}
			DataWidget* Widget()
			{
				return _widget;
			}
		};
	}
}