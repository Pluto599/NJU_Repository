package com.gameengine.recording;

/**
 * 录制服务已移除，保留空实现以兼容旧引用。
 */
public final class RecordingService {
	public RecordingService(RecordingConfig config) {
	}

	public boolean isRecording() {
		return false;
	}

	public String getOutputPath() {
		return "";
	}

	public void setStorage(RecordingStorage storage) {
	}

	public void start(Object scene, int width, int height) {
	}

	public void stop() {
	}

	public void update(double deltaTime, Object scene, Object input) {
	}
}
