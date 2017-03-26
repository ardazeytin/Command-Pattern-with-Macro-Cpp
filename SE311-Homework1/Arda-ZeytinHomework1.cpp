#include <iostream>
#include <vector>
using namespace std;
//
// The classes and/or objects participating in this pattern are:
//
// 1. Command  (Command)
//		- declares an interface for executing an operation.
// 2. ConcreteCommand  (CalculatorCommand)
//		- defines a binding between a Receiver object and an action.
//		- implements Execute by invoking the corresponding operation(s) on Receiver
// 3. Client  (Calculator Application)
//		- creates a ConcreteCommand object and sets its receiver.
// 4. Invoker  (User)
//		- asks the command to carry out the request
// 5. Receiver  (Calculator)
//		- knows how to perform the operations associated with carrying out
//		  a request. Any class may serve as a Receiver.
//

//Student Name: Arda Zeytin
//Student Number: 20130601056
//Patterns: Command, Singleton and Composite 

// "Command"
class Command {
public:
	virtual ~Command() = default;
	virtual void Execute() = 0;
	virtual void UnExecute() = 0;
protected:
	Command() {};
};

// "Receiver"
class Calculator {
private:
	float current_value;
	static Calculator *calculatorInstance;
	Calculator()
	{
		current_value = 0;
	};

public:
	//Constructor
	void Action(char _operator, float operand) 
	{
		switch (_operator) 
		{
			case '+': 
				current_value += operand; break;
			case '-': 
				current_value -= operand; break;
			case '*':
				current_value *= operand; break;
			case '/': 
				current_value /= operand; break;
		}
		cout << "Current value " << current_value << " (following "
			<< _operator << " " << operand << ")" << endl;
	}

	//Accessor method for client
	static Calculator *getInstance()
	{
		if (!calculatorInstance)
		{
			calculatorInstance = new Calculator();
		}
		return calculatorInstance;
	}

	//helper method to not multiply by zero at first command(sendyou should send 1.0f)
	void setCurrentValue(float userInputValue)
	{
		current_value = userInputValue;
	}

	float getCurrentValue() const
	{
		return current_value;
	}
};

// "ConcreteCommand"
class CalculatorCommand : public Command {
public:
	// Constructor
	CalculatorCommand(Calculator *calculator, char op, float operand)
	{
		_calculator = calculator;
		_operator = op;
		_operand = operand;
	}

	void Execute() override
	{
		_calculator->Action(_operator, _operand);
	}

	void UnExecute() override
	{
		_calculator->Action(Undo(_operator), _operand);
	}

	// Private helper function. Needed to get the inverse operation.
private:
	char Undo(char _operator) 
	{
		switch (_operator) 
		{
		case '+': 
			return '-';
		case '-': 
			return '+';
		case '*': 
			return '/';
		case '/':
			return '*';
		default:
			return ' ';
		}
	}
	Calculator*	_calculator;
	char 		_operator;
	float 		_operand;
};

//Concrete Command for Macro
class MacroCommand : public Command
{
private:
	vector<Command*> list;

public:
	void add(Command *cmd)
	{
		list.push_back(cmd);
	}

	void SaveMemory()
	{
		for (int i = 0; i < list.size(); i++)
		{
			list.erase(list.begin() + i);
		}
	}

	void Execute () override
	{
		for (int i = 0; i < list.size(); i++)
		{
			list[i]->Execute();
		}
	}

	void UnExecute () override
	{
		for (int i = 0; i < list.size(); i++)
		{
			list[i]->UnExecute();
		}
	}
};

// "Invoker"
class User 
{
public:
	User()
	{
		current = 0;
	};
	void Redo(int levels) 
	{
		cout << "\n---- Redo " << levels << " levels " << endl;
		// Perform redo operations
		for (int i = 0; i < levels; i++) 
		{
			if (current < _commands.size()) 
			{
				Command *command = _commands[current++];
				command->Execute();
			}
		}
	}
	void Undo(int levels) 
	{
		cout << "\n---- Undo " << levels << " levels " << endl;
		// Perform undo operations
		for (int i = 0; i < levels; i++) 
		{
			if (current > 0) 
			{
				Command *command = _commands[--current];
				command->UnExecute();
			}
		}
	}
	void Compute(Command *command) 
	{
		command->Execute();
		// Add command to undo list
		_commands.push_back(command);
		current++;
	}

	void SaveMemory()
	{
		for (int i = 0; i < _commands.size(); i++)
		{
			_commands.erase(_commands.begin() + i);
		}
	}

private:
	// Initializers.
	Calculator *_calculator;
	vector<Command*> _commands;
	float current; //unsigned int
};

//Default initialization
Calculator *Calculator::calculatorInstance = nullptr;

int main()
{
	User *crazyUser = new User();
	Command *commands[6];
	Calculator *mySuperCalculator = Calculator::getInstance();
	
	commands[0] = &CalculatorCommand(mySuperCalculator, '*', 1.33); //4/3
	commands[1] = &CalculatorCommand(mySuperCalculator, '*', 3.14); //pi
	commands[2] = &CalculatorCommand(mySuperCalculator, '*', 5); //radius or height
	
	MacroCommand squareOfRadius;
	mySuperCalculator->setCurrentValue(1); //stub (we can not start multiply from 0)
	squareOfRadius.add(commands[2]); //radius
	squareOfRadius.add(commands[2]); //radius
	commands[3] = &squareOfRadius;


	MacroCommand volumeOfSphere;
	mySuperCalculator->setCurrentValue(1); //stub (we can not start multiply from 0)
	volumeOfSphere.add(&squareOfRadius); //r*r macro
	volumeOfSphere.add(commands[2]); //radius
	volumeOfSphere.add(commands[0]); //4/3
	volumeOfSphere.add(commands[1]); //pi
	commands[4] = &volumeOfSphere;

	MacroCommand volumeOfCylinder;
	mySuperCalculator->setCurrentValue(1); //stub (we can not start multiply from 0)
	volumeOfCylinder.add(&squareOfRadius); //r*r macro
	volumeOfCylinder.add(commands[2]); //height
	commands[5] = &volumeOfCylinder;

	cout << "Welcome to Volume Calculator!" << endl;
	
	//User Input
	bool loop = true;
	while (loop)
	{
		cout << "-----------------------------" << endl;
		cout << "1- Sphere" << endl;
		cout << "2- Cylinder" << endl;
		cout << "3- Cube" << endl;
		cout << "-1 to EXIT" << endl;
		cout << "Please select type of object: " << endl;
		int objectType;
		cin >> objectType;
		switch (objectType)
		{
		case -1:
			loop = false;
			break;
		case 1:
			crazyUser->Compute(commands[4]);
			cout << "Macro Result = " << mySuperCalculator->getCurrentValue() << endl; //Show current value after macro 
			crazyUser->Undo(1);
			cout << "Undo Result = " << mySuperCalculator->getCurrentValue() << endl;
			crazyUser->SaveMemory(); // we have to clean _commands vector. Otherwise undo operation will cause miscalculation when user do consecutive calculations between different macros or commands
			break;
		case 2:
			crazyUser->Compute(commands[5]); //do macro
			cout << "Macro Result = " << mySuperCalculator->getCurrentValue() << endl; //Show current value after macro 
			crazyUser->Undo(1); //undo macro
			cout << "Undo Result = " << mySuperCalculator->getCurrentValue() << endl; //Show current value after macro 
			crazyUser->SaveMemory(); // we have to clean _commands vector. Otherwise undo operation will cause miscalculation when user do consecutive calculations between different macros or commands
			break;
		default:
			cout << "Please select valid number!" << endl;
			loop = true;
			break;
		}
	}

	//Kill them all!
	MacroCommand *macroKiller = new MacroCommand();
	macroKiller->SaveMemory();
	delete macroKiller, volumeOfCylinder, volumeOfSphere, squareOfRadius, mySuperCalculator, commands, crazyUser;

	return 0;
}
//Arda Zeytin