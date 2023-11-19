#pragma once

#include <QActionGroup>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QMutex>
#include <QSettings>
#include <QSizePolicy>
#include <QThread>
#include <QTimer>

#include "app_settings.h"
#include "audio_decode_thread.h"
#include "audio_effect_gl.h"
#include "audio_play_thread.h"
#include "network_url_dlg.h"
#include "play_control_window.h"
#include "player_skin.h"
#include "playlist_window.h"
#include "read_thread.h"
#include "start_play_thread.h"
#include "stopplay_waiting_thread.h"
#include "subtitle_decode_thread.h"
#include "video_decode_thread.h"
#include "video_label.h"
#include "video_play_thread.h"
#include "video_state.h"
#include "youtube_url_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

#define MaxRecentFiles 20 // maximum recent play files
#define MaxSkinStlyes 20  // maximum sytyles
#define MaxPlaylist 5     // maximum playlist numbers

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = Q_NULLPTR);
    ~MainWindow();

public:
    void start_to_play(const QString& file);
    void stop_play();
    void pause_play();
    float volume_settings(bool set = true, float vol = 0);
    inline AudioPlayThread* get_audio_play_thread() const { return m_pAudioPlayThread.get(); }
    inline VideoPlayThread* get_video_play_thread() const { return m_pVideoPlayThread.get(); }
    inline VideoStateData* get_video_state_data() const { return m_pVideoState.get(); }
    void play_mute(bool mute);
    void play_seek();
    void play_start_seek();
    void play_seek_pre();
    void play_seek_next();
    void set_volume(int volume);
    void set_play_speed();
    void show_msg_dlg(const QString& message, const QString& windowTitle = "Warning", const QString& styleSheet = "");
    bool is_playing() const;
    QString get_playingfile() const;

public slots:
    void image_ready(const QImage&);
    void subtitle_ready(const QString&);
    void audio_data(const AudioData& data);
    void open_recentFile();
    void clear_recentfiles();
    void read_packet_stopped();
    void decode_video_stopped();
    void decode_audio_stopped();
    void decode_subtitle_stopped();
    void audio_play_stopped();
    void video_play_stopped();
    void update_play_time();
    void play_started(bool ret = true);
    void play_failed(const QString& file);
    void playlist_file_saved(const QString& file);
    void set_threads();

signals:
    void stop_audio_play_thread();
    void stop_video_play_thread();
    void stop_decode_thread();
    void stop_read_packet_thread();
    void wait_stop_audio_play_thread();
    void wait_stop_video_play_thread();

private:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;

private slots:
    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_actionHelp_triggered();
    void on_actionAbout_triggered();
    void on_actionStop_triggered();
    void on_actionFullscreen_triggered();
    void on_actionHide_Play_Ctronl_triggered();
    void on_actionYoutube_triggered();
    void on_actionAspect_Ratio_triggered();
    void on_actionSystemStyle();
    void on_actionCustomStyle();
    void on_actionLoop_Play_triggered();
    void on_actionMedia_Info_triggered();
    void on_actionKeyboard_Usage_triggered();
    void on_actionPlayList_triggered();
    void on_actionOpenNetworkUrl_triggered();
    void on_actionOriginalSize_triggered();

private:
    bool start_play();
    bool playing_has_video();
    bool playing_has_audio();
    bool playing_has_subtitle();
    void update_image(const QImage&);
    void print_decodeContext(const AVCodecContext* pVideo, bool bVideo = true) const;
    void about_media_info();
    void image_cv(QImage&);
    void image_cv_geo(QImage&);
    void resize_window(int width = 800, int height = 480);
    void resize_window(const QSize& size);
    void center_window(QRect screen_rec);
    void show_fullscreen(bool bFullscreen = true);
    bool label_fullscreen();
    void hide_statusbar(bool bHide = true);
    void hide_menubar(bool bHide = true);
    void check_hide_menubar(const QPoint& pt);
    void check_hide_play_control();
    void auto_hide_play_control(bool bHide = true);
    void displayStatusMessage(const QString& message);
    void hide_play_control(bool bHide = true);
    void set_paly_control_wnd(bool set = true);
    void update_paly_control_volume();
    void update_paly_control_status();
    void update_paly_control_muted();
    void print_size() const;
    void print_screen() const;
    void keep_aspect_ratio(bool bWidth = true);
    void create_style_menu();
    inline QScreen* screen() const;
    QRect screen_rect() const;
    qreal screen_scale() const;
    QSize dislay_video_size(AVCodecContext* pCtxVideo) const;
    inline VideoLabel* get_video_label() const { return m_video_label.get(); }
    inline PlayControlWnd* get_play_control() const { return m_play_control_wnd.get(); }
    inline QObject* get_object(const QString& name) const { return findChild<QObject*>(name); }
    void create_play_control();
    void update_play_control();
    void set_volume_updown(bool bUp = true, float unit = 0.05);
    void create_recentfiles_menu();
    void set_current_file(const QString& fileName);
    void remove_recentfiles(const QString& fileName);
    void update_recentfile_actions();
    QString stripped_name(const QString& fullFileName) const;
    void save_settings();
    void read_settings();
    QString get_selected_style() const;
    void set_style_action(const QString& style);
    void clear_subtitle_str();
    void set_subtitle(const QString& str);
    void create_cv_action_group();
    void play_speed_adjust(bool up = true);
    void create_video_label();
    void update_video_label();
    void create_audio_effect();
    void start_send_data(bool bSend = true);
    void show_audio_effect(bool bShow = true);
    void play_control_key(Qt::Key key);
    void set_default_bkground();
    bool create_video_state(const QString& file);
    void delete_video_state();
    bool create_read_thread();            // read packet thread
    bool create_decode_video_thread();    // decode thread
    bool create_decode_audio_thread();    // decode audio thread
    bool create_decode_subtitle_thread(); // decode subtitle thread
    bool create_video_play_thread();      // video play thread
    bool create_audio_play_thread();      // audio play thread
    bool start_play_thread();             // this thread to avoid frezzing ui
    void all_thread_start();
    void video_seek_inc(double incr);
    void video_seek(double pos = 0, double incr = 0);
    void update_menus();
    void enable_menus(bool enable = true);
    void enable_v_menus(bool enable = true);
    void enable_a_menus(bool enable = true);
    int get_youtube_optionid() const;
    void set_youtube_optionid(int id);
    void create_avisual_action_group(); // audio visual
    bool get_avisual_format(BarHelper::VisualFormat& fmt) const;
    void popup_audio_effect();
    void set_audio_effect_format(const BarHelper::VisualFormat& fmt);
    bool start_youtube_url_thread(const YoutubeUrlDlg::YoutubeUrlData& data);
    void wait_stop_play(const QString& file);
    void create_playlist_wnd();
    void add_to_playlist(const QString& file);
    void show_playlist(bool show = true);
    void playlist_hiden();
    void hide_cursor(bool bHide = true);
    bool cursor_in_window(QWidget* pWnd);

    void create_savedPlaylists_menu();
    void remove_playlist_file(const QString& fileName);
    void update_savedPlaylists_actions();
    bool read_playlist(const QString& playlist_file, QStringList& files) const;

public slots:
    void clear_savedPlaylists();
    void open_playlist();

private:
    std::unique_ptr<Ui::MainWindow> ui;

    std::unique_ptr<ReadThread> m_pPacketReadThread;               // read packets thread
    std::unique_ptr<VideoDecodeThread> m_pDecodeVideoThread;       // decode video thread
    std::unique_ptr<AudioDecodeThread> m_pDecodeAudioThread;       // decode audio thread
    std::unique_ptr<SubtitleDecodeThread> m_pDecodeSubtitleThread; // decode subtitle thread
    std::unique_ptr<AudioPlayThread> m_pAudioPlayThread;           // audio play thread
    std::unique_ptr<VideoPlayThread> m_pVideoPlayThread;           // video play thread
    std::unique_ptr<VideoStateData> m_pVideoState;                 // sync packets
    std::unique_ptr<StartPlayThread> m_pBeforePlayThread;          // time-consuming operations before play
    std::unique_ptr<YoutubeUrlThread> m_pYoutubeUrlThread;         // youtube url parsing
    std::unique_ptr<StopWaitingThread> m_pStopplayWaitingThread;   // waiting stop play

    QString m_videoFile;
    QTimer m_timer; // mouse moving checking timer
    AppSettings m_settings;
    PlayerSkin m_skin;
    QString m_subtitle;

    std::unique_ptr<VideoLabel> m_video_label;
    std::unique_ptr<PlayControlWnd> m_play_control_wnd;
    std::unique_ptr<AudioEffectGL> m_audio_effect_wnd;
    std::unique_ptr<PlayListWnd> m_playListWnd;

private:
    std::unique_ptr<QAction> m_recentFileActs[MaxRecentFiles];
    std::unique_ptr<QAction> m_recentClear;
    std::unique_ptr<QActionGroup> m_styleActsGroup; // style menus group
    std::unique_ptr<QAction> m_styleActions[MaxSkinStlyes];
    std::unique_ptr<QActionGroup> m_CvActsGroup; // cv menus group
    std::unique_ptr<QActionGroup> m_AVisualTypeActsGroup;
    std::unique_ptr<QActionGroup> m_AVisualGrapicTypeActsGroup;
    std::unique_ptr<QAction> m_savedPlaylists[MaxPlaylist];
    std::unique_ptr<QAction> m_PlaylistsClear;
};
