#include "EventAction.hh"
#include "G4Event.hh"

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(), fRunAction(runAction)
{}

EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    // Olay (Event) başladığında yapılacak işlemler buraya yazılır
}

void EventAction::EndOfEventAction(const G4Event*)
{
    // Olay bittiğinde yapılacak işlemler buraya yazılır
}