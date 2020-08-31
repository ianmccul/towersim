// -*- C++ -*- $Id: ringingserver.cc 987 2012-07-15 01:58:28Z uqimccul $

class RingingServer::CommandDispatcherBase
{
   public:
      CommandDispatcherBase() {}

      virtual ~CommandDispatcherBase() {}

      virtual void operator()(OutCom::RingBell const& c) const = 0;

      virtual void operator()(OutCom::Finish const& c) const = 0;

      virtual void operator()(OutCom::Abort const& c) const = 0;

      virtual void operator()(OutCom::NumberOfBells const& c) const = 0;

      virtual void operator()(OutCom::Division const& c) const = 0;

      virtual void operator()(OutCom::Call const& c) const = 0;

      virtual void operator()(OutCom::CallGoMethod const& c) const = 0;

      virtual void operator()(OutCom::CallSpliceMethod const& c) const = 0;

      virtual void operator()(OutCom::LoadCall const& c) const = 0;

      virtual void operator()(OutCom::OutputError const& c) const = 0;
};

template <typename T>
class CommandDispatcher : public RingingServer::CommandDispatcherBase
{
   public:
      CommandDispatcher(T* Fwd_) : Fwd(Fwd_) {}

      virtual void operator()(OutCom::RingBell const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::Finish const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::Abort const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::NumberOfBells const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::Division const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::Call const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::CallGoMethod const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::CallSpliceMethod const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::LoadCall const& c) const
      {
	 Fwd->operator()(c);
      }

      virtual void operator()(OutCom::OutputError const& c) const
      {
	 Fwd->operator()(c);
      }

   private:
      T* Fwd;
};

template <typename T>
void
RingingServer::RegisterCommandVisitor(T* Class)
{
   CommandDispatcherBase* Dispatcher = new CommandDispatcher<T>(Class);
   // Emit the current state information
   Dispatcher->operator()(OutCom::NumberOfBells(NumBells));

   CommandSinks.push_back(Dispatcher);
   
}
