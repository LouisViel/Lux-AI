#pragma once

class IActionStrategy
{
public:
	virtual ~IActionStrategy() { }

	virtual bool canPerform() const = 0;
	virtual bool isComplete() const = 0;

	virtual void start() = 0;
	virtual void update(int turnId) = 0;
	virtual void stop() const = 0;
};