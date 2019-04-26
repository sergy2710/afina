#include <afina/coroutine/Engine.h>

namespace Afina {
namespace Coroutine {

void Engine::Store(context &ctx) {
    char FrameStartsHere;
    char *CoroutineStackEnd = &FrameStartsHere;
    if (CoroutineStackEnd > StackBottom) {
        ctx.Hight = CoroutineStackEnd;
        ctx.Low = StackBottom;
    } else {
        ctx.Hight = StackBottom;
        ctx.Low = CoroutineStackEnd;
    }
    size_t CoroutineStackSize = ctx.Hight - ctx.Low;
    char *&CtxBuffer = std::get<0>(ctx.Stack);
    size_t &CtxSize = std::get<1>(ctx.Stack);
    if ((CoroutineStackSize > CtxSize) || (CoroutineStackSize < (CtxSize << 2))) {
        if (CtxBuffer != nullptr) {
            delete CtxBuffer;
        }
        CtxSize = CoroutineStackSize;
        CtxBuffer = new char[CoroutineStackSize];
    }
    std::memcpy(CtxBuffer, ctx.Low, CoroutineStackSize);
}

void Engine::Restore(context &ctx) {
    char FrameStartsHere;
    while ((ctx.Low <= &FrameStartsHere) && (ctx.Hight >= &FrameStartsHere)) {
        Restore(ctx);
    }
    std::memcpy(ctx.Low, std::get<0>(ctx.Stack), ctx.Hight - ctx.Low);
    longjmp(ctx.Environment, 1);
}

void Engine::Enter(context &ctx) {
    if ((cur_routine != nullptr) && (cur_routine != idle_ctx)) {
        if (setjmp(cur_routine->Environment) > 0) {
            return;
        }
        Store(*cur_routine);
    }
    cur_routine = &ctx;
    Restore(ctx);
}

void Engine::yield() {

    context *cand_coroutine = alive;
    if (cand_coroutine != nullptr) {
        if (cand_coroutine == cur_routine) {
            cand_coroutine = cand_coroutine->next;
            if (cand_coroutine != nullptr) {
                Enter(*cand_coroutine);
            }
        } else {
            Enter(*cand_coroutine);
        }
    }

    if ((cur_routine != nullptr) && !(cur_routine->blocked)) {
        return;
    }

    Enter(*idle_ctx);
}

void Engine::sched(void *routine_) {
    if (routine_ == nullptr) {
        if (cur_routine != nullptr) {
            return;
        } else {
            yield();
        }
    } else if (routine_ != cur_routine) {
        Enter(*static_cast<context *>(routine_));
    }
}

void Engine::MoveCoroutine(context *&fromlist, context *&tolist, context *routine) {
    if (fromlist == tolist) {
        return;
    }
    if (routine->next != nullptr) {
        routine->next->prev = routine->prev;
    }
    if (routine->prev != nullptr) {
        routine->prev->next = routine->next;
    }
    if (routine == fromlist) {
        fromlist = fromlist->next;
    }
    routine->next = tolist;
    tolist = routine;
    if (routine->next != nullptr) {
        routine->next->prev = routine;
    }
}

void Engine::Block() {
    if (!(cur_routine->blocked)) {
        cur_routine->blocked = true;
        MoveCoroutine(alive, blocked, cur_routine);
    }
    yield();
}

void Engine::Wake(context *ctx) {
    if (ctx->blocked) {
        ctx->blocked = false;
        MoveCoroutine(blocked, alive, ctx);
    }
}

void Engine::WakeAll(void) {
    for (context *ctx = blocked; ctx != nullptr; ctx = blocked) {
        Wake(ctx);
    }
}

bool Engine::all_blocked() const { return !alive && blocked; }

Engine::context *Engine::get_cur_routine() const { return cur_routine; }

} // namespace Coroutine
} // namespace Afina
