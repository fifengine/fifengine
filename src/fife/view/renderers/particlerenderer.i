// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/structures/particleemitter.h"
#include "model/structures/particlemanager.h"
#include "view/renderers/particlerenderer.h"
%}

namespace FIFE {
	class Instance;
	class Layer;

	enum class ParticleShape { Point, Box, Circle };

	struct ParticleConfig {
		float emissionRate;
		uint32_t burstCount;
		float burstInterval;
		ParticleShape shape;
		float shapeWidth;
		float shapeHeight;
		float lifetimeMin;
		float lifetimeMax;
		float speedMin;
		float speedMax;
		float gravity;
		float drag;
		float startSize;
		float endSize;
		uint8_t startColor[4];
		uint8_t endColor[4];
		uint32_t frameCount;
		float frameDuration;
		bool localSpace;
		uint32_t maxParticles;
		bool looping;
	};

	struct ParticleSnapshot {
		bool alive;
		float x;
		float y;
		float vx;
		float vy;
		float age;
		float lifetime;
		float size;
		uint8_t rgba[4];
		uint32_t frame;
	};

	class ParticleEmitter {
	public:
		ParticleEmitter(const ParticleConfig& config);
		virtual ~ParticleEmitter();
		void setSeed(uint32_t seed);
		void update(float dt);
		size_t getAliveCount() const;
		size_t getEmittedCount() const;
		uint32_t getCapacity() const;
		uint32_t getMaxParticles() const;
		void setMaxParticles(uint32_t maxParticles);
		ParticleSnapshot getParticle(size_t slot) const;
		ExactModelCoordinate getParticleWorldPosition(size_t slot) const;
		ExactModelCoordinate getNodeWorldPosition() const;
		void attachToInstance(Instance* instance, const Point& offset);
		void attachToLayer(Layer* layer, const Point& offset);
		void attachToPoint(const Point& point);
		void detach();
		bool isDrawnOn(const Layer* layer) const;
		bool isEmissionEnabled() const;
		Instance* getAttachedInstance() const;
		void addFrame(const ImagePtr& frame);
		size_t getFrameCount() const;
		ImagePtr getFrame(uint32_t frame) const;
	};

	class ParticleManager {
	public:
		ParticleEmitter* createEmitter(const ParticleConfig& config);
		void removeEmitter(ParticleEmitter* emitter);
		size_t getEmitterCount() const;
		void resetClock();
		void setSeed(uint32_t seed);
		static ParticleManager* getInstance();
	};

	class ParticleRenderer: public RendererBase {
	public:
		virtual ~ParticleRenderer();
		std::string getName();
		void reset();
		static ParticleRenderer* getInstance(IRendererContainer* cnt);
	private:
		ParticleRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
