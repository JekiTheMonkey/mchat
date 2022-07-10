#ifndef FD_HANDLER_HPP
#define FD_HANDLER_HPP

namespace mch
{

class FdHandler
{
public:
    FdHandler(int fd, bool ownership);
    virtual ~FdHandler();

    virtual void handle(bool w, bool r) = 0;

    int getFd() const { return _fd; }
    virtual bool wantRead() const { return true; }
    virtual bool wantWrite() const { return false; }

private:
    int _fd;
    bool _ownership;
};

} // namespace mch

#endif // FD_HANDLER_HPP
