import { createServerClient } from '@supabase/ssr'
import { type Handle, redirect } from '@sveltejs/kit'
import { sequence } from '@sveltejs/kit/hooks'

import { PUBLIC_SUPABASE_URL, PUBLIC_SUPABASE_ANON_KEY } from '$env/static/public'

const supabase: Handle = async ({ event, resolve }) => {
  // Creates a Supabase client specific to this server request.
  // The Supabase client gets the Auth token from the request cookies.
  event.locals.supabase = createServerClient(PUBLIC_SUPABASE_URL, PUBLIC_SUPABASE_ANON_KEY, {
    cookies: {
      get: (key) => event.cookies.get(key),
      set: (key, value, options) => {
        event.cookies.set(key, value, { ...options, path: '/' })
      },
      remove: (key, options) => {
        event.cookies.delete(key, { ...options, path: '/' })
      },
    },
  })

  // Unlike `supabase.auth.getSession()`, which returns the session _without_
  // validating the JWT, this function also calls `getUser()` to validate the
  // JWT before returning the session.
  event.locals.safeGetSession = async () => {
    const {
      data: { session },
    } = await event.locals.supabase.auth.getSession()
    if (!session) {
      return { session: null, user: null }
    }

    const {
      data: { user },
      error,
    } = await event.locals.supabase.auth.getUser()
    if (error) {
      // JWT validation has failed
      return { session: null, user: null }
    }

    return { session, user }
  }

  return resolve(event, {
    filterSerializedResponseHeaders(name) {
      // Supabase libraries use the `content-range` and `x-supabase-api-version`
      // headers, so we need to tell SvelteKit to pass it through.
      return name === 'content-range' || name === 'x-supabase-api-version'
    },
  })
}

const authGuard: Handle = async ({ event, resolve }) => {
  // No need to check for session if it's an API request
  if (event.url.pathname.startsWith('/api')) return resolve(event);

  const { session, user } = await event.locals.safeGetSession()
  event.locals.session = session
  event.locals.user = user

  // If there's no session, redirect to the login page
  if (!event.locals.session && !event.url.pathname.startsWith('/auth')) {
    return redirect(303, '/auth')
  }

  // If there's a session, redirect to the home page if the user tries to go to
  // the login page
  if (event.locals.session && (event.url.pathname === '/auth' || event.url.pathname === '/')) {
    return redirect(303, '/dashboard')
  }

  return resolve(event)
}

export const handle: Handle = sequence(supabase, authGuard)