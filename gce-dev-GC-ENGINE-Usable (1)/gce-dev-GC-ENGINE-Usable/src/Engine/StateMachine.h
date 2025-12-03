#ifndef CORE_STATE_MACHINE_H_INCLUDED
#define CORE_STATE_MACHINE_H_INCLUDED

namespace gce
{

	class GameObject;

	using OnBegin = void(*)(GameObject* pMe);
	using OnUpdate = void(*)(GameObject* pMe);
	using OnEnd = void(*)(GameObject* pMe);

	struct StateMachine
	{
		StateMachine(GameObject* pMe);
		
		struct Condition
		{
			bool(*condition)(GameObject* pMe);
		};

		struct Transition
		{
			Vector<Condition> conditions;
			String target;
		};

		struct Action
		{
			OnBegin onBegin;
			OnUpdate onUpdate;
			OnEnd onEnd;
		};

		void AddAction(String& name, Action action = {});
		void AddAction(String& name, OnBegin pOnBegin = nullptr, OnUpdate pOnUpdate = nullptr, OnEnd pOnEnd = nullptr);

		void SetOnBeginAction(String& name, OnBegin pOnBegin);
		void SetOnUpdateAction(String& name, OnUpdate pOnUpdate);
		void SetOnEndAction(String& name, OnEnd pOnEnd);

		void AddTransition(Transition transition);
		void AddTransition(Vector<Condition>& conditions, String& target);

		void Update();

		void Transit(String const& target);

		String actualAction;

	private:

		GameObject* m_pMe = nullptr;

		String m_actualAction;

		UnorderedMap< String, Action> m_actions;
		Vector<Transition> m_transitions;

		friend class StatesSystem;
	};
}
#endif