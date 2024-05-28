import { redirect } from '@sveltejs/kit'

import type { Actions } from './$types'

import { PUBLIC_HOST } from '$env/static/public'

export const actions: Actions = {
  signup: async ({ request, locals: { supabase } }) => {
    const formData = await request.formData()
    const email = formData.get('email') as string
    const username = formData.get('username') as string
    const password = formData.get('password') as string

    const { data, error } = await supabase.auth.signUp({ email, password, options: { data: { username } } })

    if (error) {
      console.error(error)
      return redirect(303, '/auth/error')
    } else {
      return redirect(303, '/')
    }
  },
  oauth: async ({ request, locals: { supabase } }) => {
    const { data, error } = await supabase.auth.signInWithOAuth({
      provider: 'google',
      options: {
        redirectTo: `${PUBLIC_HOST}/auth/callback`,
      },
    });

    if (data.url) {
      redirect(303, data.url);
    }
  },
}