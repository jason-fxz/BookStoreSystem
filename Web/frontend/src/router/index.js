import { createRouter, createWebHistory } from 'vue-router'
import AppLayout from '@/layout/AppLayout.vue';


const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/login',
      name: 'Login',
      component: () => import('@/views/Login.vue')
    },
    {
      path: '/logout',
      name: 'Logout',
      component: () => import('@/views/Logout.vue')
    },
    {
      path: '/',
      component: AppLayout,
      meta : {
        requireAuth: true, 
      },
      children: [
        {
          path: '/',
          name: 'Home',
          component: () => import('@/views/Home.vue')
        },
        {
          path: '/search',
          name: 'Search',
          meta : { requireLevel: 1, },
          component: () => import('@/views/SearchBook.vue')
        },
        {
          path: '/purchase',
          name: 'Purchase',
          meta : { requireLevel: 1, },
          component: () => import('@/views/PurchaseBook.vue')
        },
        {
          path: '/bookmanage',
          name: 'BookManage',
          meta : { requireLevel: 3, },
          component: () => import('@/views/BookManage.vue')
        },
        {
          path: '/usermanage',
          name: 'UserManage',
          meta : { requireLevel: 3, },
          component: () => import('@/views/UserManage.vue')
        },
        {
          path: '/viewlogs',
          name: 'ViewLogs',
          meta : { requireLevel: 7, },
          component: () => import('@/views/ViewLogs.vue')
        },
      ]
    }
  ]
})

export default router
