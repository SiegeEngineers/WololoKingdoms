#include <QObject>
#include <memory>
#include <string>
#include <fs.h>
#include "libwololokingdoms/wkconverter.h"
#include "libwololokingdoms/wksettings.h"

class WKInstaller : public QObject, public WKConvertListener {
  Q_OBJECT
private:
  WKSettings settings;
  std::unique_ptr<WKConverter> converter;
public slots:
  void process();
signals:
  void finished() override;
  void log(std::string logMessage) override;
  void setInfo(std::string info) override;
  void createDialog(std::string info) override;
  void createDialog(std::string info, std::string title) override;
  void createDialog(std::string info, std::string toReplace,
                    std::string replaceWith) override;
  void setProgress(int i) override;
  void increaseProgress(int i) override;

public:
  WKInstaller(WKSettings& settings);
  void error(std::exception const& err) override;
  void installUserPatch(fs::path exePath,
                        std::vector<std::string> cliFlags) override;
};
